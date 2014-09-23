
#ifndef _ENCVELMANAGER_h
#define _ENCVELMANAGER_h


class EncVelManager
{
protected:
  long mPrevEncPos;
  int mVelocityPercent;

public:
  void Init();
  int GetVelocityPercent();
  void SetLEDs();
  void SetAllLEDsOff();
};


#endif

