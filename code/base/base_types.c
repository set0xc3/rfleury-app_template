////////////////////////////////
//~ rjf: Comparisons

function B32
Compare_U64(U64 a, U64 b, Comparison comparison)
{
    B32 result = 0;
    switch(comparison)
    {
        default: break;
        case Comparison_EqualTo:               result = (a == b); break;
        case Comparison_NotEqualTo:            result = (a != b); break;
        case Comparison_LessThan:              result = (a <  b); break;
        case Comparison_LessThanOrEqualTo:     result = (a <= b); break;
        case Comparison_GreaterThan:           result = (a >  b); break;
        case Comparison_GreaterThanOrEqualTo:  result = (a >= b); break;
    }
    return result;
}
