static inline bool vtd_iova_range_check(uint64_t iova, VTDContextEntry *ce)

{

    

    return !(iova & ~(vtd_iova_limit(ce) - 1));

}