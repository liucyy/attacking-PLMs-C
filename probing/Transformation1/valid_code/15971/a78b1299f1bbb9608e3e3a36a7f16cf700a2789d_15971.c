int cpu_signal_handler(int host_signum, void *pinfo,

                           void *puc)

{

    siginfo_t *info = pinfo;

    ucontext_t *uc = puc;

    uint32_t *pc = uc->uc_mcontext.sc_pc;

    uint32_t insn = *pc;

    int is_write = 0;



    

    switch (insn >> 26) {

    case 0x0d: 

    case 0x0e: 

    case 0x0f: 

    case 0x24: 

    case 0x25: 

    case 0x26: 

    case 0x27: 

    case 0x2c: 

    case 0x2d: 

    case 0x2e: 

    case 0x2f: 

        is_write = 1;

    }



    return handle_cpu_signal(pc, (unsigned long)info->si_addr,

                             is_write, &uc->uc_sigmask);

}