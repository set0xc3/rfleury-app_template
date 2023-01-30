#define OS_FEATURE_GFX

////////////////////////////////
//~ rjf: Includes

//- rjf: third-party
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

//- rjf: [h] dependency layers within the codebase
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "os/os_entry_point.h"
#include "render/render_inc.h"

//- rjf: [h] this layer (the main knot of the codebase)
#include "app.h"

//- rjf: [c] dependency layers within the codebase
#include "base/base_inc.c"
#include "os/os_inc.c"
#include "os/os_entry_point.c"
#include "render/render_inc.c"

////////////////////////////////
//~ rjf: Globals

global APP_State *app_state = 0;

////////////////////////////////
//~ rjf: Basic Accessors

function B32
APP_ShouldQuit(void)
{
    return app_state->should_quit;
}

function U64
APP_FrameIndex(void)
{
    return app_state->frame_idx;
}

function M_Arena *
APP_FrameArena(void)
{
    return app_state->frame_arenas[app_state->frame_idx % ArrayCount(app_state->frame_arenas)];
}

////////////////////////////////
//~ rjf: Windows

function APP_Window *
APP_WindowOpen(String8 title, Vec2S64 size)
{
    APP_Window *window = app_state->free_window;
    if(window == 0)
    {
        window = PushArrayZero(app_state->arena, APP_Window, 1);
    }
    else
    {
        StackPop(app_state->free_window);
        MemoryZeroStruct(window);
    }
    window->handle = OS_WindowOpen(title, size);
    DLLPushBack(app_state->first_window, app_state->last_window, window);
    window->render_eqp = app_state->render_backend.EquipWindow(app_state->render_eqp, window->handle);
    return window;
}

function void
APP_WindowClose(APP_Window *window)
{
    app_state->render_backend.UnequipWindow(app_state->render_eqp, window->render_eqp, window->handle);
    OS_WindowClose(window->handle);
    DLLRemove(app_state->first_window, app_state->last_window, window);
    StackPush(app_state->free_window, window);
}

function APP_Window *
APP_WindowFromHandle(OS_Handle handle)
{
    APP_Window *window = 0;
    for(APP_Window *w = app_state->first_window; w != 0; w = w->next)
    {
        if(OS_HandleMatch(handle, w->handle))
        {
            window = w;
            break;
        }
    }
    return window;
}

////////////////////////////////
//~ rjf: Entry Points

function void
APP_UpdateAndRender(void)
{
    //- rjf: begin frame
    M_ArenaClear(APP_FrameArena());
    OS_EventList events = OS_GetEvents(APP_FrameArena());
    
    //- rjf: close windows
    for(OS_Event *event = events.first;
        event != 0;
        event = event->next)
    {
        if(event->kind == OS_EventKind_WindowClose)
        {
            OS_EatEvent(&events, event);
            APP_Window *window = APP_WindowFromHandle(event->window);
            APP_WindowClose(window);
        }
    }
    
    //- rjf: app should quit if all windows are closed
    if(app_state->first_window == 0)
    {
        app_state->should_quit = 1;
    }
    
    //- rjf: update / render all windows
    for(APP_Window *w = app_state->first_window; w != 0; w = w->next)
    {
        app_state->render_backend.Begin(app_state->render_eqp, w->render_eqp, w->handle);
        {
            // NOTE(rjf): Submit R_Layer's to the backend here, for this window
            R_Layer dummy_layer = {0};
            app_state->render_backend.Submit(app_state->render_eqp, w->render_eqp, w->handle,
                                             Dim2F32(OS_ClientRectFromWindow(w->handle)),
                                             dummy_layer);
        }
        app_state->render_backend.Finish(app_state->render_eqp, w->render_eqp, w->handle);
    }
    
    //- rjf: end frame
    app_state->frame_idx += 1;
}

function void
APP_EntryPoint(void)
{
    //- rjf: initialize thread context
    TCTX tctx = MakeTCTX();
    SetTCTX(&tctx);
    
    //- rjf: set up dependency layers
    OS_Init();
    OS_InitGfx(APP_UpdateAndRender);
    
    //- rjf: initialize main application state
    M_Arena *arena = M_ArenaAlloc(Gigabytes(16));
    app_state = PushArrayZero(arena, APP_State, 1);
    app_state->arena = arena;
    for(int i = 0; i < ArrayCount(app_state->frame_arenas); i += 1)
    {
        app_state->frame_arenas[i] = M_ArenaAlloc(Gigabytes(16));
    }
    app_state->render_backend = R_BackendOpen(Str8Lit("render_opengl.dll"));
    app_state->render_eqp = app_state->render_backend.EquipOS();
    
    //- rjf: open window
    APP_WindowOpen(Str8Lit("App Template"), V2S64(1280, 720));
    
    //- rjf: main loop
    for(;!APP_ShouldQuit();)
    {
        APP_UpdateAndRender();
    }
}
