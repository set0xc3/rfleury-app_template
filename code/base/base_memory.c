#if !defined(M_IMPL_Reserve)
#error M_IMPL_Reserve must be defined to use base memory.
#endif
#if !defined(M_IMPL_Release)
#error M_IMPL_Release must be defined to use base memory.
#endif
#if !defined(M_IMPL_Commit)
#error M_IMPL_Commit must be defined to use base memory.
#endif
#if !defined(M_IMPL_Decommit)
#error M_IMPL_Decommit must be defined to use base memory.
#endif

////////////////////////////////
//~ rjf: Arena Functions

function M_Arena *
M_NilArena(void)
{
    return &m_g_nil_arena;
}

function B32
M_ArenaIsNil(M_Arena *arena)
{
    return arena == 0 || arena == &m_g_nil_arena;
}

function M_Arena *
M_ArenaAlloc(U64 cap)
{
    M_Arena *result = (M_Arena *)M_IMPL_Reserve(cap);
    M_IMPL_Commit(result, M_COMMIT_SIZE);
    result->first = result->last = result->next = result->prev = result->parent = M_NilArena();
    result->memory      = result + sizeof(M_Arena);
    result->max         = cap;
    result->pos         = sizeof(M_Arena);
    result->commit_pos  = M_COMMIT_SIZE;
    result->align       = 8;
    return result;
}

function M_Arena *
M_ArenaAllocDefault(void)
{
    return M_ArenaAlloc(Gigabytes(4));
}

function void
M_ArenaRelease(M_Arena *arena)
{
    for(M_Arena *child = arena->first, *next = 0; !M_ArenaIsNil(child); child = next)
    {
        next = child->next;
        M_ArenaRelease(child);
    }
    M_IMPL_Release(arena);
}

function void
M_ArenaPushChild(M_Arena *parent, M_Arena *new_child)
{
    DLLPushBack_NPZ(parent->first, parent->last, new_child, next, prev, M_CheckNilArena, M_SetNilArena);
}

function void *
M_ArenaPushAligned(M_Arena *arena, U64 size, U64 align)
{
    void *memory = 0;
    align = ClampBot(arena->align, align);
    
    U64 pos = arena->pos;
    U64 pos_address = IntFromPtr(arena) + pos;
    U64 aligned_address = pos_address + align - 1;
    aligned_address -= aligned_address%align;
    
    U64 alignment_size = aligned_address - pos_address;
    if (pos + alignment_size + size <= arena->max)
    {
        U8 *mem_base = (U8*)arena;
        memory = mem_base + pos + alignment_size;
        U64 new_pos = pos + alignment_size + size;
        arena->pos = new_pos;
        
        if (new_pos > arena->commit_pos)
        {
            U64 commit_grow = new_pos - arena->commit_pos;
            commit_grow += M_COMMIT_SIZE - 1;
            commit_grow -= commit_grow%M_COMMIT_SIZE;
            M_IMPL_Commit(mem_base + arena->commit_pos, commit_grow);
            arena->commit_pos += commit_grow;
        }
    }
    
    return(memory);
}

function void *
M_ArenaPush(M_Arena *arena, U64 size)
{
    return M_ArenaPushAligned(arena, size, arena->align);
}

function void *
M_ArenaPushZero(M_Arena *arena, U64 size)
{
    void *memory = M_ArenaPush(arena, size);
    MemoryZero(memory, size);
    return memory;
}

function void
M_ArenaSetPosBack(M_Arena *arena, U64 pos)
{
    pos = Max(pos, sizeof(*arena));
    if(arena->pos > pos)
    {
        arena->pos = pos;
        
        U64 decommit_pos = pos;
        decommit_pos += M_COMMIT_SIZE - 1;
        decommit_pos -= decommit_pos%M_COMMIT_SIZE;
        U64 over_committed = arena->commit_pos - decommit_pos;
        over_committed -= over_committed%M_COMMIT_SIZE;
        if(decommit_pos > 0 && over_committed >= M_DECOMMIT_THRESHOLD)
        {
            M_IMPL_Decommit((U8*)arena + decommit_pos, over_committed);
            arena->commit_pos -= over_committed;
        }
    }
}

function void
M_ArenaSetAutoAlign(M_Arena *arena, U64 align)
{
    arena->align = align;
}

function void
M_ArenaPop(M_Arena *arena, U64 size)
{
    M_ArenaSetPosBack(arena, arena->pos-size);
}

function void
M_ArenaClear(M_Arena *arena)
{
    M_ArenaPop(arena, arena->pos);
}

function U64
M_ArenaGetPos(M_Arena *arena)
{
    return arena->pos;
}

////////////////////////////////
//~ rjf: Temp

function M_Temp
M_BeginTemp(M_Arena *arena)
{
    M_Temp result = {arena, arena->pos};
    return(result);
}

function void
M_EndTemp(M_Temp temp)
{
    M_ArenaSetPosBack(temp.arena, temp.pos);
}
