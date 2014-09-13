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

Q_DEFINE_THIS_FILE

#define NUM_FLASHES 2


// various timeouts in ticks
enum TxrTimeouts {                            
  SEND_ENCODER_TOUT  = BSP_TICKS_PER_SEC / 10,     // how often to send encoder position
  FLASH_LED_TOUT = BSP_TICKS_PER_SEC / 2
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
  rollingaveragernamespace::RollingAverager averager;
  long mCurPos;
  long mPrevEncoderCnt;
  long mPos1;
  long mPos2;
  char mEncPushes;
  char mCalibrationMultiplier;

public:
  Txr() : 
  QActive((QStateHandler)&Txr::initial), 
  mFlashTimeout(FLASH_TIMEOUT_SIG), mSendTimeout(SEND_TIMEOUT_SIG)
  {
  }

protected:
  static QP::QState initial(Txr * const me, QP::QEvt const * const e);
  static QP::QState calibration(Txr * const me, QP::QEvt const * const e);
  static QP::QState flashing(Txr * const me, QP::QEvt const * const e);
  static QP::QState freeRun(Txr * const me, QP::QEvt const * const e);
  void UpdatePosition(Txr *const me);
};


static Txr l_Txr;                   // the single instance of Txr active object (local)
QActive * const AO_Txr = &l_Txr;    // the global opaque pointer


void Txr::UpdatePosition(Txr *const me)
{ 
  long curEncoderCnt = BSP_GetEncoder();
  me->mCurPos += (curEncoderCnt - me->mPrevEncoderCnt) * me->mCalibrationMultiplier;
  me->mPrevEncoderCnt = curEncoderCnt;
  BSP_UpdateRxProxy(me->mCurPos);
}

QP::QState Txr::initial(Txr * const me, QP::QEvt const * const e) {
  me->mCalibrationMultiplier = 1;
  me->mCurPos = 0;
  me->mPrevEncoderCnt = 0;
  me->subscribe(ENC_DOWN_SIG);
  me->subscribe(ENC_UP_SIG);
  me->subscribe(PLAY_MODE_SIG);
  me->subscribe(FREE_MODE_SIG);
  me->subscribe(Z_MODE_SIG);
  me->mSendTimeout.postEvery(me, SEND_ENCODER_TOUT);
  return Q_TRAN(&calibration);
}

QP::QState Txr::calibration(Txr * const me, QP::QEvt const * const e) {
  QP::QState status_;
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      //me->mSendTimeout.disarm();
      status_ = Q_HANDLED();
      break;
    }
    case SEND_TIMEOUT_SIG: 
    {
      me->UpdatePosition(me);
      status_ = Q_HANDLED(); 
      break;
    }
    case ENC_DOWN_SIG: 
    {
      if ((me->mEncPushes)++ == 0) {
        me->mPos1 = BSP_GetEncoder();
      }
      else {
        me->mPos2 = BSP_GetEncoder();
        // todo: factor this out
        if (me->mPos1 > me->mPos2) {
          long pos = me->mPos1;
          me->mPos1 = me->mPos2;
          me->mPos2 = pos;
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

QP::QState Txr::flashing(Txr * const me, QP::QEvt const * const e) {
  static char ledCnt = 0;
  QP::QState status_;
  
  switch (e->sig) {
    case Q_ENTRY_SIG: 
    {
      ENC_GREEN_LED_ON();
      me->mFlashTimeout.postEvery(me, FLASH_LED_TOUT);
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
    case FLASH_TIMEOUT_SIG: 
    {
      if (++ledCnt > NUM_FLASHES && me->mEncPushes >= 2) {
        status_ = Q_TRAN(&freeRun);
      }
      else if (ledCnt > NUM_FLASHES) {
        status_ = Q_TRAN(&calibration); 
      }
      else {
        ENC_GREEN_LED_TOGGLE();
        status_ = Q_HANDLED();
      }
      break;
    }
    default: 
    {
      status_ = Q_SUPER(&calibration);
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
      //me->mTimeout.postEvery(me, SEND_ENCODER_TOUT);
      status_ = Q_HANDLED();
      break;
    }
    case Q_EXIT_SIG: 
    {
      //me->mTimeout.disarm();
      status_ = Q_HANDLED();
      break;
    }
    case SEND_TIMEOUT_SIG: 
    {
      long pos = BSP_GetPot();
      // todo: remove magic numbers
      pos = map(pos,0,1023,me->mPos1,me->mPos2);
      pos = averager.Roll(pos);
      BSP_UpdateRxProxy(pos);
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