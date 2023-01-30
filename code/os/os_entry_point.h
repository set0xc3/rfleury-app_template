/* date = November 17th 2021 11:13 pm */

#ifndef OS_ENTRY_POINT_H
#define OS_ENTRY_POINT_H

function void APP_EntryPoint(void);

#if OS_WINDOWS
#include "win32/win32_entry_point.h"
#else
#error Entry point not defined for OS.
#endif

#endif //OS_ENTRY_POINT_H
