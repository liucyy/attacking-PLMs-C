uint32_t HELPER(rrbe)(uint32_t r1, uint64_t r2)

{

    if (r2 > ram_size) {

        return 0;

    }



    

#if 0

    env->storage_keys[r2 / TARGET_PAGE_SIZE] &= ~SK_REFERENCED;

#endif



    

    return 0;

}