#include "base/base_ctx_crack.h"

#if OS_WINDOWS
#include <windows.h>
#include "third_party/glcorearb.h"
#include "third_party/wglext.h"
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "render/render_types.h"

#include "render_opengl.h"

#if OS_WINDOWS
#include "win32/render_opengl_win32.h"
#endif

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "render/render_types.c"

#if OS_WINDOWS
#include "win32/render_opengl_win32.c"
#endif

////////////////////////////////
//~ rjf: Globals

#define GLProc(name, type) global PFNGL##type##PROC gl##name = 0;
#include "render_opengl_proc_list.inc"

per_thread TCTX gl_tctx = {0};

////////////////////////////////
//~ rjf: Helpers

function GL_OSEquip *
GL_OSEquipFromHandle(R_Handle handle)
{
    GL_OSEquip *equip = (GL_OSEquip *)handle.u64[0];
    return equip;
}

function R_Handle
GL_HandleFromOSEquip(GL_OSEquip *equip)
{
    R_Handle handle = {0};
    handle.u64[0] = (U64)equip;
    return handle;
}

function GLuint
GL_MakeShader(GLenum type, String8 code)
{
    GLuint shader = glCreateShaderProgramv(type, 1, (char **)&code.str);
    
    GLint linked = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &linked);
    if(linked == 0)
    {
        M_Temp scratch = GetScratch(0, 0);
        GLint length = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *str = PushArrayZero(scratch.arena, char, length+1);
        glGetProgramInfoLog(shader, length+1, 0, str);
        if(length != 0)
        {
            fprintf(stderr, "%s", str);
        }
        ReleaseScratch(scratch);
    }
    
    return shader;
}

function GLuint
GL_MakeProgramPipeline(String8 vertex_code, String8 fragment_code)
{
    GLuint vertex   = GL_MakeShader(GL_VERTEX_SHADER, vertex_code);
    GLuint fragment = GL_MakeShader(GL_FRAGMENT_SHADER, fragment_code);
    GLuint pipeline = 0;
    glGenProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertex);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragment);
    return pipeline;
}

////////////////////////////////
//~ rjf: Textures

function GL_Texture
GL_TextureFromHandle(R_Handle handle)
{
    GL_Texture texture = {0};
    texture.id     = (U32)handle.u32[0];
    texture.format = (U32)handle.u32[1];
    texture.size.x = (S32)handle.u32[2];
    texture.size.y = (S32)handle.u32[3];
    return texture;
}

function R_Handle
GL_HandleFromTexture(GL_Texture texture)
{
    R_Handle handle = {0};
    handle.u32[0] = (U32)texture.id;
    handle.u32[1] = (U32)texture.format;
    handle.u32[2] = (U32)texture.size.x;
    handle.u32[3] = (U32)texture.size.y;
    return handle;
}

function GLint
GL_InternalFormatFromTextureFormat2D(R_TextureFormat2D format)
{
    GLint result = 0;
    switch(format)
    {
        default:
        case R_TextureFormat2D_R8:    {result = GL_R8;}break;
        case R_TextureFormat2D_RGBA8: {result = GL_RGBA8;}break;
    }
    return result;
}

function GLint
GL_GenericFormatFromTextureFormat2D(R_TextureFormat2D format)
{
    GLint result = 0;
    switch(format)
    {
        default:
        case R_TextureFormat2D_R8:    {result = GL_RED;}break;
        case R_TextureFormat2D_RGBA8: {result = GL_RGBA;}break;
    }
    return result;
}

function GLenum
GL_BaseTypeFromTextureFormat2D(R_TextureFormat2D format)
{
    GLenum result = GL_UNSIGNED_BYTE;
    switch(format)
    {
        default:
        case R_TextureFormat2D_R8:    {result = GL_UNSIGNED_BYTE;}break;
        case R_TextureFormat2D_RGBA8: {result = GL_UNSIGNED_BYTE;}break;
    }
    return result;
}

function void
GL_DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        fprintf(stderr, "%s\n", message);
    }
}

////////////////////////////////
//~ rjf: Backend Hooks

exported R_Handle
EquipOS(void)
{
    gl_tctx = MakeTCTX();
    SetTCTX(&gl_tctx);
    
    GL_OS_EquipOS();
#define GLProc(name, type) gl##name = (PFNGL##type##PROC)GL_OS_GetProcAddress("gl" #name);
#include "render_opengl_proc_list.inc"
    
    GL_OSEquip *equip = OS_Reserve(sizeof(*equip));
    OS_Commit(equip, sizeof(*equip));
    R_Handle equip_handle = GL_HandleFromOSEquip(equip);
    
    //- rjf: set up debug callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GL_DebugCallback, equip);
    
    //- rjf: set up state
    U8 white[] = { 0xff, 0xff, 0xff, 0xff };
    R_Handle placeholder_texture = ReserveTexture2D(equip_handle, V2S32(1, 1), R_TextureFormat2D_RGBA8);
    FillTexture2D(equip_handle, placeholder_texture, V2S32(0, 0), V2S32(1, 1), Str8(white, sizeof(white)));
    
    //- rjf: fill equipment
    equip->placeholder_texture = GL_TextureFromHandle(placeholder_texture);
    glCreateVertexArrays(1, &equip->all_purpose_vao);
    glCreateBuffers(1, &equip->dynamic_scratch_buffer_32mb);
    glNamedBufferData(equip->dynamic_scratch_buffer_32mb, Megabytes(32), 0, GL_DYNAMIC_DRAW);
    return equip_handle;
}

exported R_Handle
EquipWindow(R_Handle os_equip, OS_Handle window)
{
    R_Handle result = GL_OS_EquipWindow(os_equip, window);
    return result;
}

exported void
UnequipWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
    GL_OS_UnequipWindow(os_equip, window_equip, window);
}

exported R_Handle
ReserveTexture2D(R_Handle os_equip, Vec2S32 size, R_TextureFormat2D format)
{
    GL_Texture texture = {0};
    GLenum gl_internal_fmt = GL_InternalFormatFromTextureFormat2D(format);
    glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
    glTextureStorage2D(texture.id, 1, gl_internal_fmt, size.x, size.y);
    
    switch(format)
    {
        default: break;
        case R_TextureFormat2D_R8:
        {
            GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
            glTextureParameteriv(texture.id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
        }break;
    }
    
    texture.format = format;
    texture.size = size;
    return GL_HandleFromTexture(texture);
}

exported void
FillTexture2D(R_Handle os_equip, R_Handle handle, Vec2S32 position, Vec2S32 size, String8 data)
{
    GL_Texture texture = GL_TextureFromHandle(handle);
    glTextureSubImage2D(texture.id, 0, position.x, position.y, size.x, size.y, GL_GenericFormatFromTextureFormat2D(texture.format),
                        GL_BaseTypeFromTextureFormat2D(texture.format),
                        data.str);
    glGenerateTextureMipmap(texture.id);
}

exported void
ReleaseTexture2D(R_Handle os_equip, R_Handle handle)
{
    GL_Texture texture = GL_TextureFromHandle(handle);
    glDeleteTextures(1, &texture.id);
}

exported Vec2F32
SizeFromTexture2D(R_Handle os_equip, R_Handle handle)
{
    GL_Texture texture = GL_TextureFromHandle(handle);
    return Vec2F32FromVec(texture.size);
}

exported void
Begin(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
    GL_OS_SelectWindow(os_equip, window_equip, window);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

exported void
Submit(R_Handle os_equip, R_Handle window_equip, OS_Handle window, Vec2F32 window_size, R_Layer layer)
{
    //- rjf: prep
    GL_OS_SelectWindow(os_equip, window_equip, window);
    GL_OSEquip *gl_state = GL_OSEquipFromHandle(os_equip);
    M_Temp scratch = GetScratch(0, 0);
    
    //- rjf: set up OpenGL state
    glViewport(0, 0, window_size.x, window_size.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    //- rjf: do rendering with data in `layer`
    {
        // TODO(user)
    }
    
    //- rjf: finish
    glFlush();
    ReleaseScratch(scratch);
}

exported void
Finish(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
    GL_OS_Finish(os_equip, window_equip, window);
}
