
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define MAX_VEL_LOC      0  // long
#define ACCEL_LOC        4  // long
#define MICROSTEPS_LOC   8  // int
#define ANTENNA_LOC      10 // char

class Settings
{
public:
  Settings();
  //todo: this needs to be brought out into an event loop
  void SetMaxVelocity(long val);
  long GetMaxVelocity();
  void SetAcceleration(long val);
  long GetAcceleration();
  void SetMicrosteps(int val);
  char GetMicrosteps();
  void SetAntenna(char val);
  char GetAntenna();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
