/* date = September 21st 2021 10:48 pm */

#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

typedef U32 R_TextureFormat2D;
enum
{
    R_TextureFormat2D_Null,
    R_TextureFormat2D_R8,
    R_TextureFormat2D_RGBA8,
    R_TextureFormat2D_COUNT
};

typedef union R_Handle R_Handle;
union R_Handle
{
    U64 u64[2];
    U32 u32[4];
};

typedef struct R_Glyph R_Glyph;
struct R_Glyph
{
    Rng2F32 src;
    Vec2F32 offset;
    Vec2F32 size;
    F32 advance;
};

typedef struct R_GlyphNode R_GlyphNode;
struct R_GlyphNode
{
    R_GlyphNode *hash_next;
    U32 codepoint;
    R_Glyph glyph;
};

typedef struct R_Font R_Font;
struct R_Font
{
    M_Arena *arena;
    U32 direct_map_first;
    U32 direct_map_opl;
    R_Glyph *direct_map;
    // TODO(rjf): indirect map
    F32 line_advance;
    F32 ascent;
    F32 descent;
    R_Handle texture;
};

typedef struct R_Layer R_Layer;
struct R_Layer
{
    // NOTE(rjf): Leaving this blank for the purposes of being filled in by the
    // user of the template. This struct is for all of the data that you submit
    // to the renderer backend at once.
    int x;
};

typedef R_Handle R_EquipOSFunction(void);
typedef R_Handle R_EquipWindowFunction(R_Handle os_equip, OS_Handle window);
typedef void R_UnequipWindowFunction(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
typedef R_Handle R_ReserveTexture2DFunction(R_Handle os_equip, Vec2S32 size, R_TextureFormat2D format);
typedef void R_FillTexture2DFunction(R_Handle os_equip, R_Handle texture, Vec2S32 position, Vec2S32 size, String8 data);
typedef void R_ReleaseTexture2DFunction(R_Handle os_equip, R_Handle texture);
typedef Vec2F32 R_SizeFromTexture2DFunction(R_Handle os_equip, R_Handle texture);
typedef void R_BeginFunction(R_Handle os_equip, R_Handle window_equip, OS_Handle window);
typedef void R_SubmitFunction(R_Handle os_equip, R_Handle window_equip, OS_Handle window, Vec2F32 window_size, R_Layer layer);
typedef void R_FinishFunction(R_Handle os_equip, R_Handle window_equip, OS_Handle window);

typedef struct R_Backend R_Backend;
struct R_Backend
{
    OS_Handle library;
    R_EquipOSFunction *          EquipOS;
    R_EquipWindowFunction *      EquipWindow;
    R_UnequipWindowFunction *    UnequipWindow;
    R_ReserveTexture2DFunction * ReserveTexture2D;
    R_FillTexture2DFunction *    FillTexture2D;
    R_ReleaseTexture2DFunction * ReleaseTexture2D;
    R_SizeFromTexture2DFunction *SizeFromTexture2D;
    R_BeginFunction *            Begin;
    R_SubmitFunction *           Submit;
    R_FinishFunction *           Finish;
};

function R_Handle R_HandleZero(void);
function B32 R_HandleMatch(R_Handle a, R_Handle b);
function R_Glyph R_GlyphFromFontCodepoint(R_Font font, U32 codepoint);

#endif // RENDER_TYPES_H
