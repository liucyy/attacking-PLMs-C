static void gen_store_exclusive(DisasContext *s, int rd, int rt, int rt2,

                                TCGv_i64 inaddr, int size, int is_pair)

{

    

    int fail_label = gen_new_label();

    int done_label = gen_new_label();

    TCGv_i64 addr = tcg_temp_local_new_i64();

    TCGv_i64 tmp;



    

    tcg_gen_mov_i64(addr, inaddr);

    tcg_gen_brcond_i64(TCG_COND_NE, addr, cpu_exclusive_addr, fail_label);



    tmp = tcg_temp_new_i64();

    tcg_gen_qemu_ld_i64(tmp, addr, get_mem_index(s), MO_TE + size);

    tcg_gen_brcond_i64(TCG_COND_NE, tmp, cpu_exclusive_val, fail_label);

    tcg_temp_free_i64(tmp);



    if (is_pair) {

        TCGv_i64 addrhi = tcg_temp_new_i64();

        TCGv_i64 tmphi = tcg_temp_new_i64();



        tcg_gen_addi_i64(addrhi, addr, 1 << size);

        tcg_gen_qemu_ld_i64(tmphi, addrhi, get_mem_index(s), MO_TE + size);

        tcg_gen_brcond_i64(TCG_COND_NE, tmphi, cpu_exclusive_high, fail_label);



        tcg_temp_free_i64(tmphi);

        tcg_temp_free_i64(addrhi);

    }



    

    tcg_gen_qemu_st_i64(cpu_reg(s, rt), addr, get_mem_index(s), MO_TE + size);

    if (is_pair) {

        TCGv_i64 addrhi = tcg_temp_new_i64();



        tcg_gen_addi_i64(addrhi, addr, 1 << size);

        tcg_gen_qemu_st_i64(cpu_reg(s, rt2), addrhi,

                            get_mem_index(s), MO_TE + size);

        tcg_temp_free_i64(addrhi);

    }



    tcg_temp_free_i64(addr);



    tcg_gen_movi_i64(cpu_reg(s, rd), 0);

    tcg_gen_br(done_label);

    gen_set_label(fail_label);

    tcg_gen_movi_i64(cpu_reg(s, rd), 1);

    gen_set_label(done_label);

    tcg_gen_movi_i64(cpu_exclusive_addr, -1);



}