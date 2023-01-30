#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

////////////////////////////////
//~ rjf: Limits

#if !defined(M_COMMIT_SIZE)
#define M_COMMIT_SIZE Kilobytes(4)
#endif

#if !defined(M_DECOMMIT_THRESHOLD)
#define M_DECOMMIT_THRESHOLD Kilobytes(64)
#endif

////////////////////////////////
//~ rjf: Arena

typedef struct M_Arena M_Arena;
struct M_Arena
{
    M_Arena *first;
    M_Arena *last;
    M_Arena *next;
    M_Arena *prev;
    M_Arena *parent;
    void *memory;
    U64 commit_pos;
    U64 max;
    U64 pos;
    U64 align;
};

////////////////////////////////
//~ rjf: Arena Helpers

typedef struct M_Temp M_Temp;
struct M_Temp
{
    M_Arena *arena;
    U64 pos;
};

////////////////////////////////
//~ rjf: Globals

read_only global M_Arena m_g_nil_arena =
{
    &m_g_nil_arena,
    &m_g_nil_arena,
    &m_g_nil_arena,
    &m_g_nil_arena,
    &m_g_nil_arena,
};

////////////////////////////////
//~ rjf: Arena Functions

#define M_CheckNilArena(p) (M_ArenaIsNil(p))
#define M_SetNilArena(p) ((p) = M_NilArena())
function M_Arena *M_NilArena(void);
function B32      M_ArenaIsNil(M_Arena *arena);
function M_Arena *M_ArenaAlloc(U64 cap);
function M_Arena *M_ArenaAllocDefault(void);
function void     M_ArenaRelease(M_Arena *arena);
function void     M_ArenaPushChild(M_Arena *parent, M_Arena *new_child);
function void *   M_ArenaPushAligned(M_Arena *arena, U64 size, U64 align);
function void *   M_ArenaPush(M_Arena *arena, U64 size);
function void *   M_ArenaPushZero(M_Arena *arena, U64 size);
function void     M_ArenaSetPosBack(M_Arena *arena, U64 pos);
function void     M_ArenaSetAutoAlign(M_Arena *arena, U64 align);
function void     M_ArenaPop(M_Arena *arena, U64 size);
function void     M_ArenaClear(M_Arena *arena);
function U64      M_ArenaGetPos(M_Arena *arena);
#define PushArray(a,T,c)     (T*)M_ArenaPush((a), sizeof(T)*(c))
#define PushArrayZero(a,T,c) (T*)M_ArenaPushZero((a), sizeof(T)*(c))

////////////////////////////////
//~ rjf: Temp

function M_Temp M_BeginTemp(M_Arena *arena);
function void M_EndTemp(M_Temp temp);

#endif // BASE_MEMORY_H
