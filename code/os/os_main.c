////////////////////////////////
//~ rjf: Helpers

function B32
OS_HandleMatch(OS_Handle a, OS_Handle b)
{
    return a.u64[0] == b.u64[0];
}
