uint64_t hbitmap_serialization_granularity(const HBitmap *hb)

{

    

    assert(hb->granularity < 64 - 6);



    

    return UINT64_C(64) << hb->granularity;

}