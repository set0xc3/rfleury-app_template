/* date = September 9th 2021 7:43 pm */

#ifndef OS_GFX_H
#define OS_GFX_H

////////////////////////////////
//~ rjf: Generated Code

#include "generated/os.meta.h"

////////////////////////////////
//~ rjf: Events

typedef enum OS_EventKind
{
    OS_EventKind_Null,
    OS_EventKind_WindowClose,
    OS_EventKind_WindowLoseFocus,
    OS_EventKind_Press,
    OS_EventKind_Release,
    OS_EventKind_Text,
    OS_EventKind_MouseScroll,
    OS_EventKind_DropFile,
    OS_EventKind_COUNT
}
OS_EventKind;

typedef U32 OS_Modifiers;
enum
{
    OS_Modifier_Ctrl  = (1<<0),
    OS_Modifier_Shift = (1<<1),
    OS_Modifier_Alt   = (1<<2),
};

typedef struct OS_Event OS_Event;
struct OS_Event
{
    OS_Event *next;
    OS_Event *prev;
    OS_Handle window;
    OS_EventKind kind;
    OS_Modifiers modifiers;
    OS_Key key;
    U32 character;
    Vec2F32 position;
    Vec2F32 scroll;
    String8 path;
};

typedef struct OS_EventList OS_EventList;
struct OS_EventList
{
    OS_Event *first;
    OS_Event *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Cursors

typedef enum OS_CursorKind
{
    OS_CursorKind_Null,
    OS_CursorKind_Hidden,
    OS_CursorKind_Pointer,
    OS_CursorKind_Hand,
    OS_CursorKind_WestEast,
    OS_CursorKind_NorthSouth,
    OS_CursorKind_NorthEastSouthWest,
    OS_CursorKind_NorthWestSouthEast,
    OS_CursorKind_AllCardinalDirections,
    OS_CursorKind_IBar,
    OS_CursorKind_Blocked,
    OS_CursorKind_Loading,
    OS_CursorKind_Pan,
    OS_CursorKind_COUNT
}
OS_CursorKind;

////////////////////////////////
//~ rjf: Hooks

typedef void OS_RepaintFunction(void);

////////////////////////////////
//~ rjf: Basic Helpers

function String8 OS_StringFromKey(OS_Key key);

////////////////////////////////
//~ rjf: Event Helpers

function U64           OS_CharacterFromModifiersAndKey(OS_Modifiers modifiers, OS_Key key);
function OS_CursorKind OS_CursorKindFromResizeSides(Side x, Side y);
function String8       OS_StringFromEvent(M_Arena *arena, OS_Event *event);
function B32           OS_KeyPress(OS_EventList *events, OS_Handle window, OS_Key key, OS_Modifiers mods);
function B32           OS_KeyRelease(OS_EventList *events, OS_Handle window, OS_Key key, OS_Modifiers mods);
function B32           OS_TextCodepoint(OS_EventList *events, OS_Handle window, U32 codepoint);

////////////////////////////////
//~ rjf: @os_per_backend Main API

function void OS_InitGfx(OS_RepaintFunction *repaint);
function F32 OS_DefaultRefreshRate(void);

////////////////////////////////
//~ rjf: @os_per_backend Windows

function OS_Handle OS_WindowOpen(String8 title, Vec2S64 size);
function void      OS_WindowClose(OS_Handle handle);
function B32       OS_WindowIsMaximized(OS_Handle handle);
function void      OS_WindowMinimize(OS_Handle handle);
function void      OS_WindowMaximize(OS_Handle handle);
function B32       OS_WindowIsFocused(OS_Handle handle);
function void      OS_WindowFullscreen(OS_Handle handle);
function void      OS_WindowSetIcon(OS_Handle handle, Vec2S32 size, String8 rgba_data);

function Rng2F32   OS_RectFromWindow(OS_Handle handle);
function Rng2F32   OS_ClientRectFromWindow(OS_Handle handle);
function F32       OS_DPIFromWindow(OS_Handle handle);
function Vec2F32   OS_MouseFromWindow(OS_Handle handle);

////////////////////////////////
//~ rjf: @os_per_backend Events

function OS_EventList OS_GetEvents(M_Arena *arena);
function void OS_EatEvent(OS_EventList *events, OS_Event *event);

////////////////////////////////
//~ rjf: @os_per_backend

function void OS_SetCursor(OS_CursorKind kind);

#endif // OS_GFX_H
