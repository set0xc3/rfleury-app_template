/* date = September 21st 2021 10:06 pm */

#ifndef RENDER_MAIN_H
#define RENDER_MAIN_H

////////////////////////////////
//~ rjf: Helpers

function R_Backend R_BackendOpen(String8 path);
function void R_BackendClose(R_Backend backend);
function R_Handle R_LoadTexture(R_Handle os_equip, R_Backend backend, String8 path);
function void R_UnloadTexture(R_Handle os_equip, R_Backend backend, R_Handle texture);
function R_Font R_LoadFont(R_Handle os_equip, R_Backend backend, F32 size, String8 path);
function void R_UnloadFont(R_Handle os_equip, R_Backend backend, R_Font font);
function Vec2F32 R_AdvanceFromText(R_Font font, String8 text);

#endif // RENDER_MAIN_H
