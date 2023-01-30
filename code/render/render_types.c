function R_Handle
R_HandleZero(void)
{
    R_Handle handle = {0};
    return handle;
}

function B32
R_HandleMatch(R_Handle a, R_Handle b)
{
    return a.u64[0] == b.u64[0] && a.u64[1] == b.u64[1];
}

function R_Glyph
R_GlyphFromFontCodepoint(R_Font font, U32 codepoint)
{
    R_Glyph glyph = {0};
    if(font.direct_map_first <= codepoint && codepoint < font.direct_map_opl)
    {
        glyph = font.direct_map[codepoint - font.direct_map_first];
    }
    // TODO(rjf): look up the glyph in the indirect mapping table
    return glyph;
}
