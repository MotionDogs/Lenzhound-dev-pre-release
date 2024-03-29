#ifndef Console_h
#define Console_h

#include "Settings.h"

// this shouldn't go here, but it's late and I'm tired
// and Console IS the only place it's referenced
#define CURRENT_VERSION "1.0.2"

class String;

class Console
{
public:
  Console();
  void Init();
  void Run();
private:
  void AttachCommandCallbacks();
};

// have to make these external to class because can't attach member functions
// to handlers
// todo: look into a better way
void OnCommandList();
void OnSetChannel();
void OnGetChannel();
void OnSetPALevel();
void OnGetPALevel();
void OnGetSavedPositions();
void OnGetAllValues();
int CheckBoundsInclusive(long val, long min, long max);
void PrintSuccess(long val, String param);
void OnGetVersionNumber();

#endif
