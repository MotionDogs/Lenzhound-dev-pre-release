
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define MAX_VEL_LOC       0  // long
#define ACCEL_LOC         4  // long
#define MICROSTEPS_LOC    8  // int
#define ANTENNA_LOC       10 // int
#define CHANNEL_LOC       12 // int
#define PA_LEVEL_LOC      14 // int
#define DATA_RATE_LOC     16 // int


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
  int GetMicrosteps();
  void SetAntenna(int val);
  int GetAntenna();
  void SetChannel(int val);
  int GetChannel();
  void SetPALevel(int val);
  int GetPALevel();
  void SetDataRate(int val);
  int GetDataRate();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
