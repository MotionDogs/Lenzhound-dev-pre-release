#include "Console.h"
#include <CmdMessenger.h>
#include "Settings.h"

CmdMessenger cmdMessenger = CmdMessenger(Serial);
static Settings settings;
static const char SUCCESS[] = "SUCCESS";

// This is the list of recognized commands.  
// In order to receive, attach a callback function to these events
enum
{
  COMMAND_LIST, 
  SET_MAX_VEL, 
  GET_MAX_VEL,
  SET_ACCEL,
  GET_ACCEL,
  SET_RESOLUTION,
  GET_RESOLUTION,
  SET_ANTENNA,
  GET_ANTENNA,
  SET_CHANNEL,
  GET_CHANNEL,
  SET_PALEVEL,
  GET_PALEVEL,
  SET_DATARATE,
  GET_DATARATE
};

Console::Console()
{
}

void Console::Init()
{
  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  AttachCommandCallbacks();
}

void Console::Run()
{
 // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
}

// Show available commands
void ShowCommands() 
{
  Serial.println("Available commands");
  Serial.println(" 0;                     - This command list");
  Serial.println(" 1,<max velocity>;      - Set Max Velocity");
  Serial.println(" 2;                     - Get Max Velocity");
  Serial.println(" 3,<acceleration>;      - Set Acceleration");
  Serial.println(" 4;                     - Get Acceleration");
  Serial.println(" 5,<resolution (0-3)>;  - Set Resolution");
  Serial.println(" 6;                     - Get Resolution");
  Serial.println(" 7,<antenna>;           - Set Antenna (0-integrated; 1-remote)");
  Serial.println(" 8;                     - Get Antenna");
  Serial.println(" 9,<channel>;           - Set Channel Num (0-84)");
  Serial.println(" 10;                    - Get Channel");
  Serial.println(" 11,<PA level>;         - Set Power Amp Level (0-3)");
  Serial.println(" 12;                    - Get Power Amp Level");
  Serial.println(" 13,<data rate>;        - Set Data Rate (0-2)");
  Serial.println(" 14;                    - Get Data Rate");
}

void OnUnknownCommand()
{
  Serial.println("This command is unknown!");
  ShowCommands();
}

void OnSetMaxVel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  settings.SetMaxVelocity(val);
  Serial.println(SUCCESS);
}

void OnGetMaxVel()
{
  Serial.print("Max Vel: ");
  Serial.println(settings.GetMaxVelocity());
}

void OnSetAccel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  settings.SetAcceleration(val);
  Serial.println(SUCCESS);
}

void OnGetAccel()
{
  Serial.print("Accel: ");
  Serial.println(settings.GetAcceleration());
}

void OnSetResolution()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  settings.SetMicrosteps(val);
  Serial.println(SUCCESS);
}

void OnGetResolution()
{
  Serial.print("Resolution: ");
  Serial.println(settings.GetMicrosteps());
}

void OnSetAntenna()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  settings.SetAntenna(val);
  Serial.println(SUCCESS);
}

void OnGetAntenna()
{
  Serial.print("Antenna: ");
  Serial.println(settings.GetAntenna());
}

void OnSetChannel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  settings.SetChannel(val);
  Serial.println(SUCCESS);
}

void OnGetChannel()
{
  Serial.print("Channel: ");
  Serial.println(settings.GetChannel());
}

void OnSetPALevel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  settings.SetPALevel(val);
  Serial.println(SUCCESS);
}

void OnGetPALevel()
{
  Serial.print("PA Level: ");
  Serial.println(settings.GetPALevel());
}

void OnSetDataRate()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  settings.SetDataRate(val);
  Serial.println(SUCCESS);
}

void OnGetDataRate()
{
  Serial.print("Data Rate: ");
  Serial.println(settings.GetDataRate());
}

// Callback function that shows a list of commands
void OnCommandList()
{
  ShowCommands();
}

// Callbacks define on which received commands we take action
void Console::AttachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(COMMAND_LIST, OnCommandList);
  cmdMessenger.attach(SET_MAX_VEL, OnSetMaxVel);
  cmdMessenger.attach(GET_MAX_VEL, OnGetMaxVel);
  cmdMessenger.attach(SET_ACCEL, OnSetAccel);
  cmdMessenger.attach(GET_ACCEL, OnGetAccel);
  cmdMessenger.attach(SET_RESOLUTION, OnSetResolution);
  cmdMessenger.attach(GET_RESOLUTION, OnGetResolution);
  cmdMessenger.attach(GET_ANTENNA, OnGetAntenna);
  cmdMessenger.attach(SET_ANTENNA, OnSetAntenna);
  cmdMessenger.attach(SET_CHANNEL, OnSetChannel);
  cmdMessenger.attach(GET_CHANNEL, OnGetChannel);
  cmdMessenger.attach(SET_PALEVEL, OnSetPALevel);
  cmdMessenger.attach(GET_PALEVEL, OnGetPALevel);
  cmdMessenger.attach(SET_DATARATE, OnSetDataRate);
  cmdMessenger.attach(GET_DATARATE, OnGetDataRate);
}




