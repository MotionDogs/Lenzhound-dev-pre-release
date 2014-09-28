
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define MAX_VEL_LOC       0  // long
#define ACCEL_LOC         4  // long
#define MICROSTEPS_LOC    8  // int
#define ANTENNA_LOC       10 // char
#define CHANNEL_LOC       11 // char
#define PA_LEVEL_LOC      12 // char
#define DATA_RATE_LOC     13 // char


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
  void SetChannel(char val);
  char GetChannel();
  void SetPALevel(char val);
  char GetPALevel();
  void SetDataRate(char val);
  char GetDataRate();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
