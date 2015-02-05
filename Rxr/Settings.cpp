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

int Settings::GetMicrosteps()
{
  int val;
  eeprom::ReadInt16(MICROSTEPS_LOC, &val);
  return val;
}

void Settings::SetAntenna(int val)
{
  eeprom::WriteInt16(ANTENNA_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetAntenna()
{
  int val;
  eeprom::ReadInt16(ANTENNA_LOC, &val);
  return val;
}

void Settings::SetChannel(int val)
{
  eeprom::WriteInt16(CHANNEL_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetChannel()
{
  int val;
  eeprom::ReadInt16(CHANNEL_LOC, &val);
  return val;
}

void Settings::SetPALevel(int val)
{
  eeprom::WriteInt16(PA_LEVEL_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetPALevel()
{
  int val;
  eeprom::ReadInt16(PA_LEVEL_LOC, &val);
  return val;
}

void Settings::SetDataRate(int val)
{
  eeprom::WriteInt16(DATA_RATE_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetDataRate()
{
  int val;
  eeprom::ReadInt16(DATA_RATE_LOC, &val);
  return val;
}
