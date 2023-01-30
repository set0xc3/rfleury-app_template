/* date = October 12th 2021 9:54 pm */

#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

////////////////////////////////
//~ rjf: Types

#pragma push_macro("function")
#undef function
#include <windows.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#pragma pop_macro("function")

#if LANG_CPP
#define VTblCall(obj, name, ...) (obj)->name(__VA_ARGS__)
#else
#define VTblCall(obj, name, ...) (obj)->lpVtbl->name((obj), __VA_ARGS__)
#endif

typedef union W32_Process W32_Process;
union W32_Process
{
    W32_Process *next;
    struct
    {
        HANDLE parent_read;
        PROCESS_INFORMATION info;
        OS_ProcessStatus status;
    };
};

typedef struct W32_FileFindData W32_FileFindData;
struct W32_FileFindData
{
    HANDLE handle;
    B32 returned_first;
    WIN32_FIND_DATAW find_data;
};
StaticAssert(sizeof(W32_FileFindData) <= sizeof(OS_FileIter), W32_FileFindData_Size);

////////////////////////////////
//~ rjf: Helpers

function void W32_ReadWholeBlock(HANDLE file, void *data, U64 data_len);
function void W32_WriteWholeBlock(HANDLE file, String8List data);
function OS_FileAttributes W32_FileAttributesFromFindData(WIN32_FIND_DATAW find_data);
function W32_Process* W32_ProcessAlloc(void);
function void W32_ProcessFree(W32_Process *process);
function B32 W32_ProcessIsRunning(String8 process_name);

#endif // WIN32_MAIN_H
