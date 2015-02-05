#include "Console.h"
#include <CmdMessenger.h>
#include "Settings.h"

CmdMessenger cmdMessenger = CmdMessenger(Serial);
static Settings settings;
static const char SUCCESS[] = "SUCCESS";
static const char ERROR[] = "ERROR";

// This is the list of recognized commands.  
// In order to receive, attach a callback function to these events
enum
{
  COMMAND_LIST, 
  SET_MAX_VEL, 
  SET_ACCEL,
  SET_ANTENNA,
  SET_CHANNEL,
  SET_PALEVEL,
  SET_DATARATE,
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
  Serial.println(" 2,<acceleration>;      - Set Acceleration");
  Serial.println(" 3,<antenna>;           - Set Antenna (0=integrated; 1=remote)");
  Serial.println(" 4,<channel>;           - Set Channel Num (0-84)");
  Serial.println(" 5,<PA level>;          - Set Power Amp Level (0=-18; 1=-12; 2=-6; 3=0)[dBm]");
  Serial.println(" 6,<data rate>;         - Set Data Rate (0=.250; 1=1; 2=2)[Mbps]");
  Serial.println("Current values");
  OnGetAllValues();
}

void OnUnknownCommand()
{
  Serial.println("This command is unknown!");
  ShowCommands();
}

void PrintSuccess(long val, String param)
{
  Serial.print(param);
  Serial.print(": ");
  Serial.println(val);
}

void OnGetVersionNumber()
{
  Serial.print(" Version: ");
  Serial.println(CURRENT_VERSION);
}

void OnSetMaxVel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 10, 32000)) {
    settings.SetMaxVelocity(val);
    PrintSuccess(val, "MaxVel");
  }  
}

void OnGetMaxVel()
{
  Serial.print(" Max Vel: ");
  Serial.println(settings.GetMaxVelocity());
}

void OnSetAccel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 1, 32000)) {
    settings.SetAcceleration(val);
    PrintSuccess(val, "Accel");
  }  
}

void OnGetAccel()
{
  Serial.print(" Accel: ");
  Serial.println(settings.GetAcceleration());
}

void OnSetAntenna()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 1)) {
    settings.SetAntenna(val);
    PrintSuccess(val, "Antenna");
  }  
}

void OnGetAntenna()
{
  Serial.print(" Antenna: ");
  Serial.println(settings.GetAntenna());
}

void OnSetChannel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 84)) {
    settings.SetChannel(val);
    PrintSuccess(val, "Channel");
  }  
}

void OnGetChannel()
{
  Serial.print(" Channel: ");
  Serial.println(settings.GetChannel());
}

void OnSetPALevel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 3)) {
    settings.SetPALevel(val);
    PrintSuccess(val, "PA Level");
  }  
}

void OnGetPALevel()
{
  Serial.print(" PA Level: ");
  Serial.println(settings.GetPALevel());
}

void OnSetDataRate()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 2)) {
    settings.SetDataRate(val);
    PrintSuccess(val, "Data Rate");
  }    
}

void OnGetDataRate()
{
  Serial.print(" Data Rate: ");
  Serial.println(settings.GetDataRate());
}

// Callback function that shows a list of commands
void OnCommandList()
{
  ShowCommands();
}

void OnGetAllValues()
{
  OnGetVersionNumber();
  OnGetMaxVel();
  OnGetAccel();
  OnGetAntenna();
  OnGetChannel();
  OnGetPALevel();
  OnGetDataRate();
}

int CheckBoundsInclusive(int val, int min, int max)
{
  if (val < min || val > max) {
    Serial.println(ERROR);
    return false;
  }
  return true;
}

// Callbacks define on which received commands we take action
void Console::AttachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(COMMAND_LIST, OnCommandList);
  cmdMessenger.attach(SET_MAX_VEL, OnSetMaxVel);
  cmdMessenger.attach(SET_ACCEL, OnSetAccel);
  cmdMessenger.attach(SET_ANTENNA, OnSetAntenna);
  cmdMessenger.attach(SET_CHANNEL, OnSetChannel);
  cmdMessenger.attach(SET_PALEVEL, OnSetPALevel);
  cmdMessenger.attach(SET_DATARATE, OnSetDataRate);
}




