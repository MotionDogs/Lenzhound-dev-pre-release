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
// to handlers
// todo: look into a better way
// todo: add range checks to all these
void OnCommandList();
void OnSetChannel();
void OnGetChannel();
void OnSetPALevel();
void OnGetPALevel();
void OnSetDataRate();
void OnGetDataRate();

#endif
