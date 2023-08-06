static void query_facilities(void)

{

    struct sigaction sa_old, sa_new;

    register int r0 __asm__("0");

    register void *r1 __asm__("1");

    int fail;



    memset(&sa_new, 0, sizeof(sa_new));

    sa_new.sa_handler = sigill_handler;

    sigaction(SIGILL, &sa_new, &sa_old);



    

    

    r1 = &facilities;

    asm volatile(".word 0xb2b0,0x1000"

                 : "=r"(r0) : "0"(0), "r"(r1) : "memory", "cc");



    if (got_sigill) {

        

        



        

        got_sigill = 0;

        

        asm volatile(".word 0xb908,0x0000" : "=r"(r0) : : "cc");

        if (!got_sigill) {

            facilities |= FACILITY_ZARCH_ACTIVE;

        }



        

        got_sigill = 0;

        

        r1 = &facilities;

        asm volatile(".word 0xe300,0x1000,0x0058"

                     : "=r"(r0) : "r"(r1) : "cc");

        if (!got_sigill) {

            facilities |= FACILITY_LONG_DISP;

        }



        

        got_sigill = 0;

        

        asm volatile(".word 0xc209,0x0000,0x0000" : : : "cc");

        if (!got_sigill) {

            facilities |= FACILITY_EXT_IMM;

        }



        

        got_sigill = 0;

        

        asm volatile(".word 0xc201,0x0000,0x0001");

        if (!got_sigill) {

            facilities |= FACILITY_GEN_INST_EXT;

        }

    }



    sigaction(SIGILL, &sa_old, NULL);



    

    fail = 0;

    if ((facilities & FACILITY_ZARCH_ACTIVE) == 0) {

        fprintf(stderr, "TCG: z/Arch facility is required.\n");

        fprintf(stderr, "TCG: Boot with a 64-bit enabled kernel.\n");

        fail = 1;

    }

    if ((facilities & FACILITY_LONG_DISP) == 0) {

        fprintf(stderr, "TCG: long-displacement facility is required.\n");

        fail = 1;

    }



    

    if (sizeof(void *) != 8) {

        fprintf(stderr, "TCG: 31-bit mode is not supported.\n");

        fail = 1;

    }



    if (fail) {

        exit(-1);

    }

}