#ifndef Console_h
#define Console_h

#include "Settings.h"

class Console
{
public:
  Console();
  void Init();
  void Run();
private:
  void AttachCommandCallbacks();
  //void OnSetMaxVel();
  //void OnGetMaxVel();
  //void OnUnknownCommand();
  //void OnCommandList();
  //void ShowCommands();
};

// have to make these external to class because can't attach member functions
// to handlers - todo: look into a better way
void OnSetMaxVel();
void OnGetMaxVel();
void OnUnknownCommand();
void OnCommandList();
void ShowCommands();
void OnSetAccel();
void OnGetAccel();
void OnSetDecel();
void OnGetDecel();
void OnSetAntenna();
void OnGetAntenna();

#endif
