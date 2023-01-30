#ifndef BASE_TCTX_H
#define BASE_TCTX_H

typedef struct TCTX TCTX;
struct TCTX
{
    M_Arena *arenas[2];
    char *file_name;
    U64 line_number;
};

function TCTX MakeTCTX(void);
function void SetTCTX(TCTX *tctx);
function TCTX *GetTCTX(void);

function void RegisterThreadFileAndLine_(char *file, int line);
#define RegisterFileAndLine() RegisterThreadFileAndLine_(__FILE__, __LINE__)

function M_Temp GetScratch(M_Arena **conflicts, U64 conflict_count);
#define ReleaseScratch(temp) M_EndTemp(temp)

#endif // BASE_TCTX_H
