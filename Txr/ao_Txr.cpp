//****************************************************************************
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
#include "qp_port.h"
#include "bsp.h"
#include "RollingAverager.h"
#include "Txr.h"
#include "EncVelManager.h"
#include "Arduino.h" // if I start having problems with RF24, consider removing this

Q_DEFINE_THIS_FILE


// various timeouts in ticks
enum TxrTimeouts {                            
  SEND_ENCODER_TOUT  = BSP_TICKS_PER_SEC / 100,     // how often to send encoder position
  FLASH_RATE_TOUT = BSP_TICKS_PER_SEC / 2,      // how quick to flash LED
  FLASH_DURATION_TOUT = BSP_TICKS_PER_SEC *2,            // how long to flash LED for
  ENTER_CALIBRATION_TOUT = BSP_TICKS_PER_SEC * 2    // how long to hold calibration button before reentering calibration
};

// todo: move this somewhere else or do differently
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


class Txr : 
public QP::QActive {
private:
  QTimeEvt mFlashTimeout;
  QTimeEvt mSendTimeout;
  QTimeEvt mCalibrationTimeout;
  rollingaveragernamespace::RollingAverager averager;
  float mCurPos;  // float to save partial moves needed by encoder resolution division
  long mPrevPos;  // used to prevent jitter
  long mPrevEncoderCnt;
  long mCalibrationPos1;
  long mCalibrationPos2;
  char mEncPushes;
  char mCalibrationMultiplier;
  Packet mPacket;
  long mSavedPositions[NUM_POSITION_BUTTONS];
  EncVelManager mVelocityManager;
  unsigned char mPrevPositionButtonPressed;

public:
  Txr() : 
  QActive((QStateHandler)&Txr::initial), 
  mFlashTimeout(FLASH_RATE_SIG), mSendTimeout(SEND_TIMEOUT_SIG),
  mCalibrationTimeout(CALIBRATION_SIG)
  {
  }

protected:
  static QP::QState initial(Txr * const me, QP::QEvt const * const e);
  static QP::QState uncalibrated(Txr * const me, QP::QEvt const * const e);
  static QP::QState calibrated(Txr * const me, QP::QEvt const * const e);
  static QP::QState flashing(Txr * const me, QP::QEvt const * const e);
  static QP::QState freeRun(Txr * const me, QP::QEvt const * const e);
  static QP::QState playBack(Txr * const me, QP::QEvt const * const e);
  void UpdatePosition(Txr *const me);
  void UpdatePositionCalibration(Txr *const me);  
  void UpdatePositionPlayBack(Txr *const me);  
};


static Txr l_Txr;                   // the single instance of Txr active object (local)
QActive * const AO_Txr = &l_Txr;    // the global opaque pointer


void Txr::UpdatePositionCalibration(Txr *const me)
{ 
  long curEncoderCnt = BSP_GetEncoder();

  // since it's 4 counts per detent, let's make it a detent per motor count
  // this is a test, remove if not necessary
  float amountToMove = curEncoderCnt - me->mPrevEncoderCnt;
  amountToMove /= 4.0;
  amountToMove *= me->mCalibrationMultiplier;
  me->mCurPos += amountToMove; //(curEncoderCnt - me->mPrevEncoderCnt) * me->mCalibrationMultiplier;
  me->mPrevEncoderCnt = curEncoderCnt;

  me->mPacket.position = me->mCurPos;
  BSP_UpdateRxProxy(me->mPacket);
}

void Txr::UpdatePosition(Txr *const me)
{
  long newPos = BSP_GetPot();
  // map the Position from Pot values to calibrated motor values
  newPos = map(newPos, MIN_POT_VAL, MAX_POT_VAL, me->mCalibrationPos1, me->mCalibrationPos2);
  newPos = me->averager.Roll(newPos);
  
  // only update the current position if it's not jittering between two values
  if (newPos != me->mPrevPos && newPos != me->mCurPos) {
    me->mPrevPos = me->mCurPos;
    me->mCurPos = newPos;
  }
  
  me->mPacket.position = me->mCurPos;
  BSP_UpdateRxProxy(me->mPacket);
}

void Txr::UpdatePositionPlayBack(Txr *const me)
{
  me->mPacket.position = me->mCurPos;
  me->mPacket.velocity = me->mVelocityManager.GetVelocityPercent();
  me->mVelocityManager.SetLEDs();
  BSP_UpdateRxProxy(me->mPacket);
}

QP::QState Txr::initial(Txr * const me, QP::QEvt const * const e) {
  me->mCalibrationMultiplier = 1;
  me->mCurPos = 0;
  me->mPrevEncoderCnt = 0;
  me->mPacket.position = 0;
  me->subscribe(ENC_DOWN_SIG);
  me->subscribe(ENC_UP_SIG);
  me->subscribe(PLAY_MODE_SIG);
  me->subscribe(FREE_MODE_SIG);
  me->subscribe(Z_MODE_SIG);
  me->subscribe(POSITION_BUTTON_SIG);
  me->mSendTimeout.postEvery(me, SEND_ENCODER_TOUT);
  return Q_TRAN(&uncalibrated);
}

QP::QState Txr::uncalibrated(Txr * const me, QP::QEvt const * const e) {
  QP::QState status_;
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      me->mPacket.mode = FREE_MODE;
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      status_ = Q_HANDLED();
      break;
    }
    case SEND_TIMEOUT_SIG: 
    {
      me->UpdatePositionCalibration(me);
      status_ = Q_HANDLED(); 
      break;
    }
    case ENC_DOWN_SIG: 
    {
      // if this is first time button press, just save the position
      if ((me->mEncPushes)++ == 0) {
        me->mCalibrationPos1 = me->mCurPos;
      }
      // if this is second time, determine whether swapping is necessary
      // to map higher calibrated position with higher motor position
      else {
        me->mCalibrationPos2 = me->mCurPos;
        if (me->mCalibrationPos1 > me->mCalibrationPos2) {
          long pos = me->mCalibrationPos1;
          me->mCalibrationPos1 = me->mCalibrationPos2;
          me->mCalibrationPos2 = pos;
        }
      }
      status_ = Q_TRAN(&flashing);
      break;
    }
    case PLAY_MODE_SIG: 
    {
      me->mCalibrationMultiplier = 10;
      status_ = Q_HANDLED(); 
      break;
    }
    case Z_MODE_SIG: 
    {
      me->mCalibrationMultiplier = 5;
      status_ = Q_HANDLED(); 
      break;
    }
    case FREE_MODE_SIG: 
    {
      me->mCalibrationMultiplier = 1;
      status_ = Q_HANDLED(); 
      break;
    }
    default: 
    {
      status_ = Q_SUPER(&QP::QHsm::top);
      break;
    }
  }
  return status_;
}

QP::QState Txr::calibrated(Txr * const me, QP::QEvt const * const e) {
  QP::QState status_;
  switch (e->sig) {
    case Q_ENTRY_SIG:
    {
      // set all saved positions to within calibrated range
      for (int i=0; i<NUM_POSITION_BUTTONS; i++) {
        me->mSavedPositions[i] = me->mCalibrationPos1;
      }
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG:
    {
      status_ = Q_HANDLED();
      break;
    }
    case ENC_DOWN_SIG:
    {
      // reenter calibration if held down for 2 seconds
      me->mCalibrationTimeout.postIn(me, ENTER_CALIBRATION_TOUT);
      status_ = Q_HANDLED();
      break;
    }
    case ENC_UP_SIG:
    {
      // if they released the button before the time is up, stop waiting for the timeout
      me->mCalibrationTimeout.disarm();
      status_ = Q_HANDLED();
      break;
    }
    case CALIBRATION_SIG:
    {
      // we've held for 2 seconds, go to calibration
      me->mEncPushes = 0;
      status_ = Q_TRAN(&flashing);
      break;
    }
    case PLAY_MODE_SIG:
    {
      status_ = Q_TRAN(&playBack);
      break;
    }
    case Z_MODE_SIG:
    {
      status_ = Q_TRAN(&freeRun);
      break;
    }
    case FREE_MODE_SIG:
    {
      status_ = Q_TRAN(&freeRun);
      break;
    }
    default:
    {
      status_ = Q_SUPER(&QP::QHsm::top);
      break;
    }
  }
  return status_;
}

QP::QState Txr::flashing(Txr * const me, QP::QEvt const * const e) {
  static char ledCnt = 0;
  QP::QState status_;
  
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      ENC_GREEN_LED_TOGGLE();
      me->mFlashTimeout.postEvery(me, FLASH_RATE_TOUT);
      me->mCalibrationTimeout.postIn(me, FLASH_DURATION_TOUT);
      ledCnt = 0;
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      me->mFlashTimeout.disarm();
      // leave light on if calibration complete so user knows
      if (me->mEncPushes >= 2) {
        ENC_GREEN_LED_ON();
      }
      else {
        ENC_GREEN_LED_OFF();
      }
      status_ = Q_HANDLED();
      break;
    }
    case CALIBRATION_SIG: 
    {
      // if they've pressed button 2 times calibration should be complete
      if (me->mEncPushes >= 2) {
        if (FREESWITCH_ON()) {
          status_ = Q_TRAN(&freeRun);
        }  
        else {        
          status_ = Q_TRAN(&playBack);
        }        
      }
      else {
        status_ = Q_TRAN(&uncalibrated);
      }
      break;
    }
    case FLASH_RATE_SIG: 
    {
      ENC_GREEN_LED_TOGGLE();
      status_ = Q_HANDLED();
      break;
    }
    default: 
    {
      status_ = Q_SUPER(&uncalibrated);
      break;
    }
  }
  return status_;
}

QP::QState Txr::freeRun(Txr * const me, QP::QEvt const * const e) {
  QP::QState status_;
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      me->mPacket.mode = FREE_MODE;
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      status_ = Q_HANDLED();
      break;
    }
    case SEND_TIMEOUT_SIG: 
    {
      me->UpdatePosition(me);            
      status_ = Q_HANDLED(); 
      break;
    }
    case POSITION_BUTTON_SIG:
    {
      me->mPrevPositionButtonPressed = ((PositionButtonEvt*)e)->ButtonNum;
      Q_REQUIRE(me->mPrevPositionButtonPressed < NUM_POSITION_BUTTONS);
      me->mSavedPositions[me->mPrevPositionButtonPressed] = me->mCurPos;
      
      // flash LED if not already flashing another LED
      if (me->mFlashTimeout.ctr() == 0) {
        BSP_TurnOnSpeedLED(me->mPrevPositionButtonPressed);
        me->mFlashTimeout.postIn(me, FLASH_RATE_TOUT);
      }
            
      status_ = Q_HANDLED();
      break;
    }
    case FLASH_RATE_SIG: 
    {
      // turn off flashed LED
      BSP_TurnOffSpeedLED(me->mPrevPositionButtonPressed);
      status_ = Q_HANDLED();
      break;
    }
    default: 
    {
      status_ = Q_SUPER(&calibrated);
      break;
    }
  }
  return status_;
}

QP::QState Txr::playBack(Txr * const me, QP::QEvt const * const e) {
  QP::QState status_;
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      me->mPacket.mode = PLAYBACK_MODE;
      me->mPacket.position = me->mCurPos;
      me->mVelocityManager.Init();
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      me->mVelocityManager.SetAllLEDsOff();
      status_ = Q_HANDLED();
      break;
    }
    case SEND_TIMEOUT_SIG: 
    {
      me->UpdatePositionPlayBack(me);            
      status_ = Q_HANDLED(); 
      break;
    }
    case POSITION_BUTTON_SIG:
    {
      int buttonNum = ((PositionButtonEvt*)e)->ButtonNum;
      Q_REQUIRE(buttonNum < NUM_POSITION_BUTTONS);
      me->mCurPos = me->mSavedPositions[buttonNum];
      status_ = Q_HANDLED();
      break;
    }
    default: 
    {
      status_ = Q_SUPER(&calibrated);
      break;
    }
  }
  return status_;
}



