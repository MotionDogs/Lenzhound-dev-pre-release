// 
// Class used to track encoder position as it relates to setting
// velocity in PlayBack mode
// 

#include "EncVelManager.h"
#include "Arduino.h"
#include "bsp.h"

// These are percentages
#define MAX_VELOCITY  100
#define MIN_VELOCITY  1

#define ENC_CNTS_TO_VEL_PERCENT  2


void EncVelManager::Init()
{
  // start at mid-speed
  mVelocityPercent = 50;
  mPrevEncPos = BSP_GetEncoder();
}

int EncVelManager::GetVelocityPercent()
{
  long curEncPos = BSP_GetEncoder();
  mVelocityPercent += (curEncPos - mPrevEncPos) / ENC_CNTS_TO_VEL_PERCENT;
  mPrevEncPos = curEncPos;
  mVelocityPercent = min(mVelocityPercent, MAX_VELOCITY);
  mVelocityPercent = max(mVelocityPercent, MIN_VELOCITY);
  return mVelocityPercent;
}

void EncVelManager::SetLEDs()
{
  SetAllLEDsOff();
  
  if (mVelocityPercent > 85) {
    GREEN_LED_ON();
  }
  else if (mVelocityPercent > 70 && mVelocityPercent <= 85) {
    AMBER2_LED_ON();
    GREEN_LED_ON();
  }
  else if (mVelocityPercent > 55 && mVelocityPercent <= 70) {
    AMBER2_LED_ON();
  }
  else if (mVelocityPercent > 40 && mVelocityPercent <= 55) {
    AMBER_LED_ON();
    AMBER2_LED_ON();
  }
  else if (mVelocityPercent > 25 && mVelocityPercent <= 40) {
    AMBER_LED_ON();
  }
  else if (mVelocityPercent > 10 && mVelocityPercent <= 25) {
    RED_LED_ON();
    AMBER_LED_ON();
  }
  else {
    RED_LED_ON();
  }
  
}

void EncVelManager::SetAllLEDsOff()
{
  RED_LED_OFF();
  AMBER_LED_OFF();
  AMBER2_LED_OFF();
  GREEN_LED_OFF();
}