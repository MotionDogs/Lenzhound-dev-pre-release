#include "qp_port.h"
#include "Console.h"
#include "Txr.h"
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
  COMMAND_LIST = 0,
  SET_CHANNEL = 9,
  GET_CHANNEL,
  SET_PALEVEL,
  GET_PALEVEL,
  SET_DATARATE,
  GET_DATARATE,
  GET_ALL_VALS
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
  Serial.println(" 9,<channel>;           - Set Channel Num (0-84)");
  Serial.println(" 10;                    - Get Channel");
  Serial.println(" 11,<PA level>;         - Set Power Amp Level (0-3)");
  Serial.println(" 12;                    - Get Power Amp Level");
  Serial.println(" 13,<data rate>;        - Set Data Rate (0-2)");
  Serial.println(" 14;                    - Get Data Rate");
  Serial.println(" 15;                    - Get All");
}

void OnUnknownCommand()
{
  Serial.println("This command is unknown!");
  ShowCommands();
}

void OnSetChannel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 84)) {
    settings.SetChannel(val);
    Serial.println(SUCCESS);
    QF::PUBLISH(Q_NEW(QEvt, UPDATE_PARAMS_SIG), &OnSetChannel);
  }  
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
  if (CheckBoundsInclusive(val, 0, 3)) {
    settings.SetPALevel(val);
    Serial.println(SUCCESS);
    QF::PUBLISH(Q_NEW(QEvt, UPDATE_PARAMS_SIG), &OnSetPALevel);
  }  
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
  if (CheckBoundsInclusive(val, 0, 2)) {
    settings.SetDataRate(val);
    Serial.println(SUCCESS);
    QF::PUBLISH(Q_NEW(QEvt, UPDATE_PARAMS_SIG), &OnSetDataRate);
  }    
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

void OnGetAllValues()
{
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
  cmdMessenger.attach(SET_CHANNEL, OnSetChannel);
  cmdMessenger.attach(GET_CHANNEL, OnGetChannel);
  cmdMessenger.attach(SET_PALEVEL, OnSetPALevel);
  cmdMessenger.attach(GET_PALEVEL, OnGetPALevel);
  cmdMessenger.attach(SET_DATARATE, OnSetDataRate);
  cmdMessenger.attach(GET_DATARATE, OnGetDataRate);
  cmdMessenger.attach(GET_ALL_VALS, OnGetAllValues);
}




