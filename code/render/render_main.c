////////////////////////////////
//~ rjf: Helpers

function R_Backend
R_BackendOpen(String8 path)
{
    R_Backend backend = {0};
    backend.library = OS_LibraryOpen(path);
    backend.EquipOS           =          (R_EquipOSFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("EquipOS"));
    backend.EquipWindow       =      (R_EquipWindowFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("EquipWindow"));
    backend.UnequipWindow     =    (R_UnequipWindowFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("UnequipWindow"));
    backend.ReserveTexture2D  = (R_ReserveTexture2DFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("ReserveTexture2D"));
    backend.FillTexture2D     =    (R_FillTexture2DFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("FillTexture2D"));
    backend.ReleaseTexture2D  = (R_ReleaseTexture2DFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("ReleaseTexture2D"));
    backend.SizeFromTexture2D = (R_SizeFromTexture2DFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("SizeFromTexture2D"));
    backend.Begin             =            (R_BeginFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("Begin"));
    backend.Submit            =           (R_SubmitFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("Submit"));
    backend.Finish            =           (R_FinishFunction *)OS_LibraryLoadFunction(backend.library, Str8Lit("Finish"));
    return backend;
}

function void
R_BackendClose(R_Backend backend)
{
    OS_LibraryClose(backend.library);
}

function R_Handle
R_LoadTexture(R_Handle os_equip, R_Backend backend, String8 path)
{
    R_Handle texture = R_HandleZero();
    M_Temp scratch = GetScratch(0, 0);
    String8 data = OS_LoadEntireFile(scratch.arena, path);
    if(data.size != 0)
    {
        int width = 0;
        int height = 0;
        int components = 0;
        U8 *pixels = stbi_load_from_memory(data.str, data.size, &width, &height, &components, 4);
        if(width != 0 && height != 0 && pixels != 0)
        {
            texture = backend.ReserveTexture2D(os_equip, V2S32(width, height), R_TextureFormat2D_RGBA8);
            backend.FillTexture2D(os_equip, texture, V2S32(0, 0), V2S32(width, height), Str8(pixels, width*height*4));
            stbi_image_free(pixels);
        }
    }
    ReleaseScratch(scratch);
    return texture;
}

function void
R_UnloadTexture(R_Handle os_equip, R_Backend backend, R_Handle texture)
{
    backend.ReleaseTexture2D(os_equip, texture);
}

function R_Font
R_LoadFont(R_Handle os_equip, R_Backend backend, F32 size, String8 path)
{
    // rjf: pre-requisites
    M_Temp scratch = GetScratch(0, 0);
    
    // rjf: constants
    U32 direct_map_first = 32;
    U32 direct_map_opl = 128;
    Vec2F32 oversample = { 1.f, 1.f };
    Vec2S32 atlas_size = V2S32(1024, 1024);
    
    // rjf: load file
    String8 font_data = OS_LoadEntireFile(scratch.arena, path);
    
    // rjf: allocate
    M_Arena *arena = M_ArenaAlloc(Megabytes(256));
    U8 *pixels = PushArrayZero(arena, U8, atlas_size.x*atlas_size.y);
    
    // rjf: calculate basic metrics
    F32 ascent = 0;
    F32 descent = 0;
    F32 line_gap = 0;
    stbtt_GetScaledFontVMetrics(font_data.str, 0, size, &ascent, &descent, &line_gap);
    F32 line_advance = ascent - descent + line_gap;
    
    // rjf: pack
    stbtt_pack_context ctx = {0};
    stbtt_PackBegin(&ctx, pixels, atlas_size.x, atlas_size.y, 0, 2, 0);
    stbtt_PackSetOversampling(&ctx, oversample.x, oversample.y);
    stbtt_packedchar *chardata_for_range = PushArrayZero(scratch.arena, stbtt_packedchar, direct_map_opl-direct_map_first);
    stbtt_pack_range rng =
    {
        size,
        (int)direct_map_first,
        0,
        (int)(direct_map_opl - direct_map_first),
        chardata_for_range,
    };
    stbtt_PackFontRanges(&ctx, font_data.str, 0, &rng, 1);
    stbtt_PackEnd(&ctx);
    
    // rjf: build direct map
    R_Glyph *direct_map = PushArrayZero(arena, R_Glyph, direct_map_opl-direct_map_first);
    for(U32 codepoint = direct_map_first; codepoint < direct_map_opl; codepoint += 1)
    {
        U32 index = codepoint - direct_map_first;
        F32 x_offset = 0;
        F32 y_offset = 0;
        stbtt_aligned_quad quad = {0};
        stbtt_GetPackedQuad(rng.chardata_for_range, atlas_size.x, atlas_size.y, index, &x_offset, &y_offset, &quad, 1);
        R_Glyph *glyph = direct_map + index;
        glyph->src = R2F32(V2F32(quad.s0 * atlas_size.x,
                                 quad.t0 * atlas_size.y),
                           V2F32(quad.s1 * atlas_size.x,
                                 quad.t1 * atlas_size.y));
        glyph->offset = V2F32(quad.x0, quad.y0);
        glyph->size = Mul2F32(Dim2F32(glyph->src), V2F32(1.f / oversample.x, 1.f / oversample.y));;
        glyph->advance = x_offset;
    }
    
    // rjf: fill + return
    R_Font font = {0};
    font.arena = arena;
    font.direct_map_first = direct_map_first;
    font.direct_map_opl = direct_map_opl;
    font.direct_map = direct_map;
    font.texture = backend.ReserveTexture2D(os_equip, atlas_size, R_TextureFormat2D_R8);
    font.line_advance = line_advance;
    font.ascent = ascent;
    font.descent = descent;
    backend.FillTexture2D(os_equip, font.texture, V2S32(0, 0), atlas_size, Str8(pixels, atlas_size.x*atlas_size.y));
    ReleaseScratch(scratch);
    return font;
}

function void
R_UnloadFont(R_Handle os_equip, R_Backend backend, R_Font font)
{
    backend.ReleaseTexture2D(os_equip, font.texture);
    M_ArenaRelease(font.arena);
}

function Vec2F32
R_AdvanceFromText(R_Font font, String8 text)
{
    Vec2F32 v = {0};
    for(U64 offset = 0; offset<text.size;)
    {
        DecodedCodepoint decode = DecodeCodepointFromUtf8(text.str+offset, text.size-offset);
        U32 codepoint = decode.codepoint;
        if(decode.advance == 0)
        {
            break;
        }
        offset += decode.advance;
        
        R_Glyph glyph = R_GlyphFromFontCodepoint(font, codepoint);
        v.x += glyph.advance;
    }
    v.y += font.line_advance;
    return v;
}
