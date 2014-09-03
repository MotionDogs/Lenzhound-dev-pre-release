
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define MAX_VEL_LOC      0  // long
#define ACCEL_LOC        4  // long
#define DECEL_LOC        8  // long
#define ANTENNA_LOC      12 // char

class Settings
{
public:
  Settings();
  
  void SetMaxVelocity(long val);
  long GetMaxVelocity();
  void SetAcceleration(long val);
  long GetAcceleration();
  void SetDeceleration(long val);
  long GetDeceleration();
  void SetAntenna(char val);
  char GetAntenna();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
