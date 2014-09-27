#include "Settings.h"
#include "eepromImpl.h"
#include "events.h"

Settings::Settings() {
  // if values are saved might to load them up here  
}

void Settings::SetMaxVelocity(long val)
{
  eeprom::WriteInt32(MAX_VEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetMaxVelocity()
{
  long val;
  eeprom::ReadInt32(MAX_VEL_LOC, &val);
  return val;
}

void Settings::SetAcceleration(long val)
{
  eeprom::WriteInt32(ACCEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetAcceleration()
{
  long val;
  eeprom::ReadInt32(ACCEL_LOC, &val);
  return val;
}

void Settings::SetMicrosteps(int val)
{
  eeprom::WriteInt16(MICROSTEPS_LOC, val); 
  events::set_dirty(true);
}

char Settings::GetMicrosteps()
{
  int val;
  eeprom::ReadInt16(MICROSTEPS_LOC, &val);
  return (char)val;
}

void Settings::SetAntenna(char val)
{
  eeprom::WriteChar(ANTENNA_LOC, val); 
  events::set_dirty(true);
}

char Settings::GetAntenna()
{
  char val;
  eeprom::ReadChar(ANTENNA_LOC, &val);
  return val;
}
