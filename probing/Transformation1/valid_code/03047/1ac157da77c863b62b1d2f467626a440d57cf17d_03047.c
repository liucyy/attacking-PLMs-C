void cpu_x86_set_a20(CPUX86State *env, int a20_state)

{

    a20_state = (a20_state != 0);

    if (a20_state != a20_enabled) {

#if defined(DEBUG_MMU)

        printf("A20 update: a20=%d\n", a20_state);

#endif

        

        cpu_interrupt(env, 0);



        

        tlb_flush(env);

        a20_enabled = a20_state;

        if (a20_enabled)

            a20_mask = 0xffffffff;

        else

            a20_mask = 0xffefffff;

    }

}