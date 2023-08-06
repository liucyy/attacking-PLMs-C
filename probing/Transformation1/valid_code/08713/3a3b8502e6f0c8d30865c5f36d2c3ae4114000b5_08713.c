target_ulong spapr_rtas_call(PowerPCCPU *cpu, sPAPREnvironment *spapr,

                             uint32_t token, uint32_t nargs, target_ulong args,

                             uint32_t nret, target_ulong rets)

{

    if ((token >= TOKEN_BASE)

        && ((token - TOKEN_BASE) < TOKEN_MAX)) {

        struct rtas_call *call = rtas_table + (token - TOKEN_BASE);



        if (call->fn) {

            call->fn(cpu, spapr, token, nargs, args, nret, rets);

            return H_SUCCESS;

        }

    }



    

    if (token == 0xa) {

        rtas_display_character(cpu, spapr, 0xa, nargs, args, nret, rets);

        return H_SUCCESS;

    }



    hcall_dprintf("Unknown RTAS token 0x%x\n", token);

    rtas_st(rets, 0, RTAS_OUT_PARAM_ERROR);

    return H_PARAMETER;

}