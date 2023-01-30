/* date = October 12th 2021 9:54 pm */

#ifndef WIN32_GFX_H
#define WIN32_GFX_H

////////////////////////////////
//~ rjf: Types

typedef struct W32_Window W32_Window;
struct W32_Window
{
    W32_Window *next;
    W32_Window *prev;
    HWND hwnd;
    HDC hdc;
    B32 last_window_placement_initialized;
    WINDOWPLACEMENT last_window_placement;
};

typedef UINT W32_GetDpiForWindowType(HWND hwnd);

////////////////////////////////
//~ rjf: Helpers

function OS_Handle W32_HandleFromWindow(W32_Window *window);
function W32_Window *W32_WindowFromHandle(OS_Handle handle);
function W32_Window *W32_WindowOpen(String8 title, Vec2S64 size);
function void W32_WindowClose(W32_Window *window);
function OS_Modifiers W32_GetModifiers(void);

////////////////////////////////
//~ rjf: Window Proc

function LRESULT W32_WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

#endif // WIN32_GFX_H
