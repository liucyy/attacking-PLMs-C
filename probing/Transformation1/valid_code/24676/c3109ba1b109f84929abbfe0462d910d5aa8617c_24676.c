int load_flt_binary(struct linux_binprm * bprm, struct target_pt_regs * regs,

                    struct image_info * info)

{

    struct lib_info libinfo[MAX_SHARED_LIBS];

    abi_ulong p = bprm->p;

    abi_ulong stack_len;

    abi_ulong start_addr;

    abi_ulong sp;

    int res;

    int i, j;



    memset(libinfo, 0, sizeof(libinfo));

    

    stack_len = 0;

    for (i = 0; i < bprm->argc; ++i) {

        

        stack_len += strlen(bprm->argv[i]);

    }

    for (i = 0; i < bprm->envc; ++i) {

        

        stack_len += strlen(bprm->envp[i]);

    }

    stack_len += (bprm->argc + 1) * 4; 

    stack_len += (bprm->envc + 1) * 4; 





    res = load_flat_file(bprm, libinfo, 0, &stack_len);

    if (res > (unsigned long)-4096)

            return res;



    

    for (i=0; i<MAX_SHARED_LIBS; i++) {

        if (libinfo[i].loaded) {

            abi_ulong p;

            p = libinfo[i].start_data;

            for (j=0; j<MAX_SHARED_LIBS; j++) {

                p -= 4;

                

                if (put_user_ual(libinfo[j].loaded

                                 ? libinfo[j].start_data

                                 : UNLOADED_LIB,

                                 p))

                    return -EFAULT;

            }

        }

    }



    p = ((libinfo[0].start_brk + stack_len + 3) & ~3) - 4;

    DBG_FLT("p=%x\n", (int)p);



    

    p = copy_strings(p, bprm->envc, bprm->envp);

    p = copy_strings(p, bprm->argc, bprm->argv);

    

    sp = p & ~(abi_ulong)(sizeof(abi_ulong) - 1);

    

    stack_len = bprm->envc + bprm->argc + 2;

    stack_len += 3;	

    stack_len *= sizeof(abi_ulong);

    if ((sp + stack_len) & 15)

        sp -= 16 - ((sp + stack_len) & 15);

    sp = loader_build_argptr(bprm->envc, bprm->argc, sp, p, 1);



    

    start_addr = libinfo[0].entry;



#ifdef CONFIG_BINFMT_SHARED_FLAT

#error here

    for (i = MAX_SHARED_LIBS-1; i>0; i--) {

            if (libinfo[i].loaded) {

                    

                    --sp;

                    if (put_user_ual(start_addr, sp))

                        return -EFAULT;

                    start_addr = libinfo[i].entry;

            }

    }

#endif



    

    info->start_code = libinfo[0].start_code;

    info->end_code = libinfo[0].start_code = libinfo[0].text_len;

    info->start_data = libinfo[0].start_data;

    info->end_data = libinfo[0].end_data;

    info->start_brk = libinfo[0].start_brk;

    info->start_stack = sp;

    info->stack_limit = libinfo[0].start_brk;

    info->entry = start_addr;

    info->code_offset = info->start_code;

    info->data_offset = info->start_data - libinfo[0].text_len;



    DBG_FLT("start_thread(entry=0x%x, start_stack=0x%x)\n",

            (int)info->entry, (int)info->start_stack);



    return 0;

}