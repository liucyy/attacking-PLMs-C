static inline int get_a32_user_mem_index(DisasContext *s)

{

    

    switch (s->mmu_idx) {

    case ARMMMUIdx_S1E2:        

    case ARMMMUIdx_S12NSE0:

    case ARMMMUIdx_S12NSE1:

        return arm_to_core_mmu_idx(ARMMMUIdx_S12NSE0);

    case ARMMMUIdx_S1E3:

    case ARMMMUIdx_S1SE0:

    case ARMMMUIdx_S1SE1:

        return arm_to_core_mmu_idx(ARMMMUIdx_S1SE0);




    case ARMMMUIdx_S2NS:

    default:

        g_assert_not_reached();

    }

}