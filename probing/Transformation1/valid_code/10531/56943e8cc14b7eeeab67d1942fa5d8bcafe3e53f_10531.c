void tcg_cpu_address_space_init(CPUState *cpu, AddressSpace *as)

{

    

    assert(cpu->as == as);



    if (cpu->cpu_ases) {

        

        return;

    }



    cpu->cpu_ases = g_new0(CPUAddressSpace, 1);

    cpu->cpu_ases[0].cpu = cpu;

    cpu->cpu_ases[0].as = as;

    cpu->cpu_ases[0].tcg_as_listener.commit = tcg_commit;

    memory_listener_register(&cpu->cpu_ases[0].tcg_as_listener, as);

}