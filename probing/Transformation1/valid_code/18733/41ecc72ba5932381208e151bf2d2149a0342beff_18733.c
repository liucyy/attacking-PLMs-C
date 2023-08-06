static int setup_sigcontext(struct target_sigcontext *sc,

                            CPUOpenRISCState *regs,

                            unsigned long mask)

{

    int err = 0;

    unsigned long usp = regs->gpr[1];



    



    



    

    



    

    __put_user(mask, &sc->oldmask);

    __put_user(usp, &sc->usp); return err;

}