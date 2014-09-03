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
  SET_DECEL,
  GET_DECEL,
  SET_ANTENNA,
  GET_ANTENNA
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
  Serial.println(" 5,<deceleration>;      - Set Deceleration");
  Serial.println(" 6;                     - Get Deceleration");
  Serial.println(" 7,<antenna>;           - Set Antenna (0-integrated; 1-remote)");
  Serial.println(" 8;                     - Get Antenna");
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

void OnSetDecel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  settings.SetDeceleration(val);
  Serial.println(SUCCESS);
}

void OnGetDecel()
{
  Serial.print("Decel: ");
  Serial.println(settings.GetDeceleration());
}

void OnSetAntenna()
{
  // todo: what happens if there is no arg?
  char val = cmdMessenger.readCharArg();
  settings.SetAntenna(val);
  Serial.println(SUCCESS);
}

void OnGetAntenna()
{
  Serial.print("Antenna: ");
  Serial.println(settings.GetAntenna());
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
  cmdMessenger.attach(SET_DECEL, OnSetDecel);
  cmdMessenger.attach(GET_DECEL, OnGetDecel);
  cmdMessenger.attach(GET_ANTENNA, OnGetAntenna);
  cmdMessenger.attach(SET_ANTENNA, OnSetAntenna);
}




