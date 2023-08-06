static void jump_to_IPL_code(uint64_t address)

{

    

    write_subsystem_identification();

    

    ResetInfo *current = 0;



    save = *current;

    current->ipl_addr = (uint32_t) (uint64_t) &jump_to_IPL_2;

    current->ipl_continue = address & 0x7fffffff;



    debug_print_int("set IPL addr to", current->ipl_continue);



    

    sclp_print("\n");



    

    asm volatile("lghi 1,1\n\t"

                 "diag 1,1,0x308\n\t"

                 : : : "1", "memory");

    virtio_panic("\n! IPL returns !\n");

}