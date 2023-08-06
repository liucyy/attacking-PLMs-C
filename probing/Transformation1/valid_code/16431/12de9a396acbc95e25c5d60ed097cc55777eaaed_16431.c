static int check_physical (CPUState *env, mmu_ctx_t *ctx,

                           target_ulong eaddr, int rw)

{

    int in_plb, ret;



    ctx->raddr = eaddr;

    ctx->prot = PAGE_READ;

    ret = 0;

    switch (env->mmu_model) {

    case POWERPC_MMU_32B:

    case POWERPC_MMU_SOFT_6xx:

    case POWERPC_MMU_SOFT_74xx:

    case POWERPC_MMU_601:

    case POWERPC_MMU_SOFT_4xx:

    case POWERPC_MMU_REAL_4xx:

    case POWERPC_MMU_BOOKE:

        ctx->prot |= PAGE_WRITE;

        break;

#if defined(TARGET_PPC64)

    case POWERPC_MMU_64B:

    case POWERPC_MMU_64BRIDGE:

        

        ctx->raddr &= 0x0FFFFFFFFFFFFFFFULL;

        ctx->prot |= PAGE_WRITE;

        break;

#endif

    case POWERPC_MMU_SOFT_4xx_Z:

        if (unlikely(msr_pe != 0)) {

            

            in_plb =

                

                (env->pb[0] < env->pb[1] &&

                 

                 eaddr >= env->pb[0] && eaddr < env->pb[1]) ||

                (env->pb[2] < env->pb[3] &&

                 eaddr >= env->pb[2] && eaddr < env->pb[3]) ? 1 : 0;

            if (in_plb ^ msr_px) {

                

                if (rw == 1) {

                    

                    ret = -2;

                }

            } else {

                

                ctx->prot |= PAGE_WRITE;

            }

        }

        break;

    case POWERPC_MMU_BOOKE_FSL:

        

        cpu_abort(env, "BookE FSL MMU model not implemented\n");

        break;

    default:

        cpu_abort(env, "Unknown or invalid MMU model\n");

        return -1;

    }



    return ret;

}