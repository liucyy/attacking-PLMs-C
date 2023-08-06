bool is_tcg_gen_code(uintptr_t tc_ptr)

{

    

    return (tc_ptr >= (uintptr_t)tcg_ctx.code_gen_buffer &&

            tc_ptr < (uintptr_t)(tcg_ctx.code_gen_buffer +

                    tcg_ctx.code_gen_buffer_max_size));

}