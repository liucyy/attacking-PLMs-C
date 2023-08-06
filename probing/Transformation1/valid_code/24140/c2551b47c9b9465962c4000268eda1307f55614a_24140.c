static size_t refcount_array_byte_size(BDRVQcow2State *s, uint64_t entries)

{

    

    assert(entries < (UINT64_C(1) << (64 - 9)));



    

    return DIV_ROUND_UP(entries << s->refcount_order, 8);

}