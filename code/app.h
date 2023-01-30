#ifndef APP_H
#define APP_H

typedef struct APP_Window APP_Window;
struct APP_Window
{
    APP_Window *next;
    APP_Window *prev;
    OS_Handle handle;
    R_Handle render_eqp;
};

typedef struct APP_State APP_State;
struct APP_State
{
    M_Arena *arena;
    M_Arena *frame_arenas[2];
    B32 should_quit;
    U64 frame_idx;
    APP_Window *first_window;
    APP_Window *last_window;
    APP_Window *free_window;
    R_Backend render_backend;
    R_Handle render_eqp;
};

////////////////////////////////
//~ rjf: Basic Accessors

function B32 APP_ShouldQuit(void);
function U64 APP_FrameIndex(void);
function M_Arena *APP_FrameArena(void);

////////////////////////////////
//~ rjf: Windows

function APP_Window *APP_WindowOpen(String8 title, Vec2S64 size);
function void APP_WindowClose(APP_Window *window);
function APP_Window *APP_WindowFromHandle(OS_Handle handle);

////////////////////////////////
//~ rjf: Entry Points

function void APP_UpdateAndRender(void);
function void APP_EntryPoint(void);

#endif // APP_H
