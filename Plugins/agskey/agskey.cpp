/*

Fake a keypress

*/

#ifdef WIN32
#define WINDOWS_VERSION
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(disable : 4244)
#endif

#if !defined(BUILTIN_PLUGINS)
#define THIS_IS_THE_PLUGIN
#endif

#include "plugin/agsplugin.h"

#if defined(BUILTIN_PLUGINS)
namespace agskey {
#endif

IAGSEngine* engine;


#if defined(IOS_VERSION)

extern "C"
{
  void fakekey(int keypress);
}

#endif



// ********************************************
// ************  AGS Interface  ***************
// ********************************************


void FakeKeypress(int keypress)
{
#if defined(IOS_VERSION)
  fakekey(keypress);
#endif
}

void AGS_EngineStartup(IAGSEngine *lpEngine)
{
  engine = lpEngine;

  engine->RegisterScriptFunction("FakeKeypress", (void*)&FakeKeypress);
}

void AGS_EngineShutdown()
{

}

int AGS_EngineOnEvent(int event, int data)
{
  return 0;
}

int AGS_EngineDebugHook(const char *scriptName, int lineNum, int reserved)
{
  return 0;
}

void AGS_EngineInitGfx(const char *driverID, void *data)
{
}



#if defined(WINDOWS_VERSION) && !defined(BUILTIN_PLUGINS)

// ********************************************
// ***********  Editor Interface  *************
// ********************************************

const char* scriptHeader =
  "import void FakeKeypress(int);\r\n"

  ;


IAGSEditor* editor;


LPCSTR AGS_GetPluginName(void)
{
  // Return the plugin description
  return "Fake key press";
}

int  AGS_EditorStartup(IAGSEditor* lpEditor)
{
  // User has checked the plugin to use it in their game

  // If it's an earlier version than what we need, abort.
  if (lpEditor->version < 1)
    return -1;

  editor = lpEditor;
  editor->RegisterScriptHeader(scriptHeader);

  // Return 0 to indicate success
  return 0;
}

void AGS_EditorShutdown()
{
  // User has un-checked the plugin from their game
  editor->UnregisterScriptHeader(scriptHeader);
}

void AGS_EditorProperties(HWND parent)
{
  // User has chosen to view the Properties of the plugin
  // We could load up an options dialog or something here instead
  MessageBoxA(parent, "Fake key press plugin by Janet Gilbert", "About", MB_OK | MB_ICONINFORMATION);
}

int AGS_EditorSaveGame(char* buffer, int bufsize)
{
  // We don't want to save any persistent data
  return 0;
}

void AGS_EditorLoadGame(char* buffer, int bufsize)
{
  // Nothing to load for this plugin
}

#endif


#if defined(BUILTIN_PLUGINS)
}
#endif