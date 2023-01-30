/* date = September 8th 2021 10:58 pm */

#ifndef OS_MAIN_H
#define OS_MAIN_H

#define M_IMPL_Reserve  OS_Reserve
#define M_IMPL_Release  OS_Release
#define M_IMPL_Commit   OS_Commit
#define M_IMPL_Decommit OS_Decommit

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
    U64 u64[1];
};

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
    U8 opaque[1024];
};

typedef U32 OS_FileFlags;
enum
{
    OS_FileFlag_Directory = (1<<0),
};

typedef struct OS_FileAttributes OS_FileAttributes;
struct OS_FileAttributes
{
    OS_FileFlags flags;
    U64 size;
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{
    String8 name;
    OS_FileAttributes attributes;
};

typedef enum OS_SystemPath
{
    OS_SystemPath_Null,
    OS_SystemPath_Initial,
    OS_SystemPath_Current,
    OS_SystemPath_Binary,
    OS_SystemPath_AppData,
    OS_SystemPath_COUNT,
} OS_SystemPath;

typedef struct OS_ProcessStatus OS_ProcessStatus;
struct OS_ProcessStatus
{
    B8 launch_failed;
    B8 running;
    B8 read_failed;
    B8 kill_failed;
    B8 was_killed;
    U32 exit_code;
};

////////////////////////////////
//~ rjf: Helpers

function B32 OS_HandleMatch(OS_Handle a, OS_Handle b);

////////////////////////////////
//~ rjf: @os_per_backend General Program API

function void    OS_Init(void);
function void    OS_Abort(void);
function String8 OS_GetSystemPath(M_Arena *arena, OS_SystemPath path);

////////////////////////////////
//~ rjf: @os_per_backend Memory

function U64   OS_PageSize(void);
function void *OS_Reserve(U64 size);
function void  OS_Release(void *ptr);
function void  OS_Commit(void *ptr, U64 size);
function void  OS_Decommit(void *ptr, U64 size);

////////////////////////////////
//~ rjf: @os_per_backend Libraries

function OS_Handle     OS_LibraryOpen(String8 path);
function void          OS_LibraryClose(OS_Handle handle);
function VoidFunction *OS_LibraryLoadFunction(OS_Handle handle, String8 name);

////////////////////////////////
//~ rjf: @os_per_backend File System

function String8           OS_LoadEntireFile(M_Arena *arena, String8 path);
function B32               OS_SaveToFile(String8 path, String8List data);
function void              OS_DeleteFile(String8 path);
function B32               OS_MakeDirectory(String8 path);
function OS_FileIter *     OS_FileIterBegin(M_Arena *arena, String8 path);
function B32               OS_FileIterNext(M_Arena *arena, OS_FileIter *it, OS_FileInfo *out_info);
function void              OS_FileIterEnd(OS_FileIter *it);
function OS_FileAttributes OS_FileAttributesFromPath(String8 path);

////////////////////////////////
//~ rjf: @os_per_backend Time

function U64 OS_TimeMicroseconds(void);
function U64 OS_TimeCycles(void);
function void OS_Sleep(U64 milliseconds);

////////////////////////////////
//~ rjf: @os_per_backend Threads

function U64 OS_GetTID(void);

////////////////////////////////
//~ rjf: @os_per_backend Child Processes

function OS_Handle        OS_ProcessLaunch(String8 command, String8 working_directory);
function void             OS_ProcessRelease(OS_Handle handle);
function String8          OS_ProcessReadOutput(M_Arena *arena, OS_Handle process);
function void             OS_ProcessKill(OS_Handle process);
function U64              OS_PIDFromProcess(OS_Handle process);
function OS_ProcessStatus OS_StatusFromProcess(OS_Handle process);

////////////////////////////////
//~ rjf: @os_per_backend Miscellaneous

function void OS_GetEntropy(void *data, U64 size);
function void OS_OpenCodeFileInDevTools(String8 path, int line);
function F32 OS_CaretBlinkTime(void);

#endif // OS_MAIN_H
