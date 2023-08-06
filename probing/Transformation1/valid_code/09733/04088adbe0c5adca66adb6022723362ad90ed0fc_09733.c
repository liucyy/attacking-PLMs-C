static void bamboo_init(ram_addr_t ram_size,

                        const char *boot_device,

                        const char *kernel_filename,

                        const char *kernel_cmdline,

                        const char *initrd_filename,

                        const char *cpu_model)

{

    unsigned int pci_irq_nrs[4] = { 28, 27, 26, 25 };

    PCIBus *pcibus;

    CPUState *env;

    uint64_t elf_entry;

    uint64_t elf_lowaddr;

    target_phys_addr_t entry = 0;

    target_phys_addr_t loadaddr = 0;

    target_long kernel_size = 0;

    target_ulong initrd_base = 0;

    target_long initrd_size = 0;

    target_ulong dt_base = 0;

    void *fdt;

    int i;



    

    env = ppc440ep_init(&ram_size, &pcibus, pci_irq_nrs, 1, cpu_model);



    if (pcibus) {

        

        for (i = 0; i < nb_nics; i++) {

            

            pci_nic_init_nofail(&nd_table[i], "e1000", NULL);

        }

    }



    

    if (kernel_filename) {

        kernel_size = load_uimage(kernel_filename, &entry, &loadaddr, NULL);

        if (kernel_size < 0) {

            kernel_size = load_elf(kernel_filename, 0, &elf_entry, &elf_lowaddr,

                                   NULL, 1, ELF_MACHINE, 0);

            entry = elf_entry;

            loadaddr = elf_lowaddr;

        }

        

        if (kernel_size < 0) {

            fprintf(stderr, "qemu: could not load kernel '%s'\n",

                    kernel_filename);

            exit(1);

        }

    }



    

    if (initrd_filename) {

        initrd_base = kernel_size + loadaddr;

        initrd_size = load_image_targphys(initrd_filename, initrd_base,

                                          ram_size - initrd_base);



        if (initrd_size < 0) {

            fprintf(stderr, "qemu: could not load initial ram disk '%s'\n",

                    initrd_filename);

            exit(1);

        }

    }



    

    if (kernel_filename) {

        if (initrd_base)

            dt_base = initrd_base + initrd_size;

        else

            dt_base = kernel_size + loadaddr;



        fdt = bamboo_load_device_tree(dt_base, ram_size,

                                      initrd_base, initrd_size, kernel_cmdline);

        if (fdt == NULL) {

            fprintf(stderr, "couldn't load device tree\n");

            exit(1);

        }



        cpu_synchronize_state(env);



        

        env->gpr[1] = (16<<20) - 8;

        env->gpr[3] = dt_base;

        env->nip = entry;

        

    }



    if (kvm_enabled())

        kvmppc_init();

}