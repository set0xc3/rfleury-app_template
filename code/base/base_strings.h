#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#define STB_SPRINTF_DECORATE(name) ts_stbsp_##name
#include "third_party/stb_sprintf.h"

typedef struct String8 String8;
struct String8
{
    U8 *str;
    U64 size;
};

typedef struct String16 String16;
struct String16
{
    U16 *str;
    U64 size;
};

typedef struct String32 String32;
struct String32
{
    U32 *str;
    U64 size;
};

typedef struct String8Node String8Node;
struct String8Node
{
    String8Node *next;
    String8 string;
};

typedef struct String8List String8List;
struct String8List
{
    String8Node *first;
    String8Node *last;
    U64 node_count;
    U64 total_size;
};

typedef struct StringJoin StringJoin;
struct StringJoin
{
    String8 pre;
    String8 sep;
    String8 post;
};

typedef U32 MatchFlags;
enum
{
    MatchFlag_CaseInsensitive  = (1<<0),
    MatchFlag_RightSideSloppy  = (1<<1),
    MatchFlag_SlashInsensitive = (1<<2),
    MatchFlag_FindLast         = (1<<3),
};

typedef struct DecodedCodepoint DecodedCodepoint;
struct DecodedCodepoint
{
    U32 codepoint;
    U32 advance;
};

typedef enum IdentifierStyle
{
    IdentifierStyle_UpperCamelCase,
    IdentifierStyle_LowerCamelCase,
    IdentifierStyle_UpperCase,
    IdentifierStyle_LowerCase,
}
IdentifierStyle;

////////////////////////////////
//~ rjf: Char Functions

function B32 CharIsAlpha(U8 c);
function B32 CharIsAlphaUpper(U8 c);
function B32 CharIsAlphaLower(U8 c);
function B32 CharIsDigit(U8 c);
function B32 CharIsSymbol(U8 c);
function B32 CharIsSpace(U8 c);
function U8  CharToUpper(U8 c);
function U8  CharToLower(U8 c);
function U8  CharToForwardSlash(U8 c);

////////////////////////////////
//~ rjf: String Functions

//- rjf: Helpers
function U64 CalculateCStringLength(char *cstr);

//- rjf: Basic Constructors
function String8 Str8(U8 *str, U64 size);
#define Str8C(cstring) Str8((U8 *)(cstring), CalculateCStringLength(cstring))
#define Str8Lit(s) Str8((U8 *)(s), sizeof(s)-1)
#define Str8LitComp(s) {(U8 *)(s), sizeof(s)-1}
function String8 Str8Range(U8 *first, U8 *one_past_last);
function String16 Str16(U16 *str, U64 size);
function String16 Str16C(U16 *ptr);
#define Str8Struct(ptr) Str8((U8 *)(ptr), sizeof(*(ptr)))

//- rjf: Substrings
function String8 Substr8(String8 str, U64 min, U64 max);
function String8 Str8Skip(String8 str, U64 min);
function String8 Str8Chop(String8 str, U64 nmax);
function String8 Prefix8(String8 str, U64 size);
function String8 Suffix8(String8 str, U64 size);

//- rjf: Matching
function B32 Str8Match(String8 a, String8 b, MatchFlags flags);
function U64 FindSubstr8(String8 haystack, String8 needle, U64 start_pos, MatchFlags flags);

//- rjf: Allocation
function String8 PushStr8Copy(M_Arena *arena, String8 string);
function String8 PushStr8FV(M_Arena *arena, char *fmt, va_list args);
function String8 PushStr8F(M_Arena *arena, char *fmt, ...);

//- rjf: Use In Format Strings
#define Str8VArg(s) (int)(s).size, (s).str

//- rjf: String Lists
function void Str8ListPushNode(String8List *list, String8Node *n);
function void Str8ListPush(M_Arena *arena, String8List *list, String8 str);
function void Str8ListConcat(String8List *list, String8List *to_push);
function String8List StrSplit8(M_Arena *arena, String8 string, int split_count, String8 *splits);
function String8 Str8ListJoin(M_Arena *arena, String8List list, StringJoin *optional_params);

//- rjf: String Re-Styling
function String8 Str8Stylize(M_Arena *arena, String8 string, IdentifierStyle style, String8 separator);

////////////////////////////////
//~ rjf: Unicode Conversions

function DecodedCodepoint DecodeCodepointFromUtf8(U8 *str, U64 max);
function DecodedCodepoint DecodeCodepointFromUtf16(U16 *str, U64 max);
function U32              Utf8FromCodepoint(U8 *out, U32 codepoint);
function U32              Utf16FromCodepoint(U16 *out, U32 codepoint);
function String8          Str8From16(M_Arena *arena, String16 str);
function String16         Str16From8(M_Arena *arena, String8 str);
function String8          Str8From32(M_Arena *arena, String32 str);
function String32         Str32From8(M_Arena *arena, String8 str);

////////////////////////////////
//~ rjf: Skip/Chop Helpers

function String8 Str8ChopLastPeriod(String8 str);
function String8 Str8SkipLastSlash(String8 str);
function String8 Str8SkipLastPeriod(String8 str);
function String8 Str8ChopLastSlash(String8 str);

////////////////////////////////
//~ rjf: Numeric Conversions

function U64 U64FromStr8(String8 str, U32 radix);
function S64 CStyleIntFromStr8(String8 str);
function F64 F64FromStr8(String8 str);
function String8 CStyleHexStringFromU64(M_Arena *arena, U64 x, B32 caps);

#endif // BASE_STRINGS_H
