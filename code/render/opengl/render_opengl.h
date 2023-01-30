/* date = September 21st 2021 10:52 pm */

#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

typedef struct GL_Texture GL_Texture;
struct GL_Texture
{
    U32 id;
    R_TextureFormat2D format;
    Vec2S32 size;
};

typedef struct GL_OSEquip GL_OSEquip;
struct GL_OSEquip
{
    GL_Texture placeholder_texture;
    GLuint all_purpose_vao;
    GLuint dynamic_scratch_buffer_32mb;
};

////////////////////////////////
//~ rjf: Helpers

function GL_OSEquip *GL_OSEquipFromHandle(R_Handle handle);
function R_Handle GL_HandleFromOSEquip(GL_OSEquip *equip);
function GLuint GL_MakeProgramPipeline(String8 vertex, String8 fragment);

////////////////////////////////
//~ rjf: Textures

function GL_Texture GL_TextureFromHandle(R_Handle handle);
function R_Handle GL_HandleFromTexture(GL_Texture texture);
function GLint GL_InternalFormatFromTextureFormat2D(R_TextureFormat2D format);
function GLint GL_GenericFormatFromTextureFormat2D(R_TextureFormat2D format);
function GLenum GL_BaseTypeFromTextureFormat2D(R_TextureFormat2D format);
function void GL_DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

////////////////////////////////
//~ rjf: Per-OS Implementations

function VoidFunction *GL_OS_GetProcAddress(char *name);
function void GL_OS_EquipOS(void);
function R_Handle GL_OS_EquipWindow(R_Handle os_equip, OS_Handle window);
function void GL_OS_UnequipWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
function void GL_OS_SelectWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
function void GL_OS_Finish(R_Handle os_equip, R_Handle window_equip, OS_Handle window);

////////////////////////////////
//~ rjf: Backend Hooks

exported R_Handle EquipOS(void);
exported R_Handle EquipWindow(R_Handle os_equip, OS_Handle window);
exported void UnequipWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
exported R_Handle ReserveTexture2D(R_Handle os_equip, Vec2S32 size, R_TextureFormat2D format);
exported void FillTexture2D(R_Handle os_equip, R_Handle texture, Vec2S32 position, Vec2S32 size, String8 data);
exported void ReleaseTexture2D(R_Handle os_equip, R_Handle texture);
exported Vec2F32 SizeFromTexture2D(R_Handle os_equip, R_Handle texture);
exported void Begin(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
exported void Submit(R_Handle os_equip, R_Handle window_equip, OS_Handle window, Vec2F32 window_size, R_Layer layer);
exported void Finish(R_Handle os_equip, R_Handle window_equip, OS_Handle window);

#endif // RENDER_OPENGL_H
