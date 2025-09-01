#ifdef _WIN32
#include <windows.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BluetoothAPIs.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib, "Bthprops.lib")
#else

#endif

void pair_wiimotes();