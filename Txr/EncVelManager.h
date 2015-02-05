
#ifndef _ENCVELMANAGER_h
#define _ENCVELMANAGER_h


class EncVelManager
{
protected:
  long mPrevEncPos;
  int mVelocityPercent;
  int mPrevVelocityPercent;

public:
  void Init();
  int GetVelocityPercent();
  void SetLEDs();
  void SetAllLEDsOff();
};


#endif

