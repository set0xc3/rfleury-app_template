function String8
Str8FromMD(MD_String8 str)
{
    String8 result = { str.str, str.size };
    return result;
}

function MD_String8
MDFromStr8(String8 str)
{
    MD_String8 result = { str.str, str.size };
    return result;
}
