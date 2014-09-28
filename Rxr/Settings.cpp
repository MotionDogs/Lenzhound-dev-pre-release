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

void Settings::SetChannel(char val)
{
  eeprom::WriteChar(CHANNEL_LOC, val); 
}

char Settings::GetChannel()
{
  char val;
  eeprom::ReadChar(CHANNEL_LOC, &val);
  return val;
}

void Settings::SetPALevel(char val)
{
  eeprom::WriteChar(PA_LEVEL_LOC, val); 
}

char Settings::GetPALevel()
{
  char val;
  eeprom::ReadChar(PA_LEVEL_LOC, &val);
  return val;
}

void Settings::SetDataRate(char val)
{
  eeprom::WriteChar(DATA_RATE_LOC, val); 
}

char Settings::GetDataRate()
{
  char val;
  eeprom::ReadChar(DATA_RATE_LOC, &val);
  return val;
}
