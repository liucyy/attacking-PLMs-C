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

    target_long initrd_size = 0;

    int success;

    int i;



    

    env = ppc440ep_init(&ram_size, &pcibus, pci_irq_nrs, 1, cpu_model);



    if (pcibus) {

        

        for (i = 0; i < nb_nics; i++) {

            

            pci_nic_init_nofail(&nd_table[i], "e1000", NULL);

        }

    }



    

    if (kernel_filename) {

        success = load_uimage(kernel_filename, &entry, &loadaddr, NULL);

        if (success < 0) {

            success = load_elf(kernel_filename, NULL, NULL, &elf_entry,

                               &elf_lowaddr, NULL, 1, ELF_MACHINE, 0);

            entry = elf_entry;

            loadaddr = elf_lowaddr;

        }

        

        if (success < 0) {

            fprintf(stderr, "qemu: could not load kernel '%s'\n",

                    kernel_filename);

            exit(1);

        }

    }



    

    if (initrd_filename) {

        initrd_size = load_image_targphys(initrd_filename, RAMDISK_ADDR,

                                          ram_size - RAMDISK_ADDR);



        if (initrd_size < 0) {

            fprintf(stderr, "qemu: could not load ram disk '%s' at %x\n",

                    initrd_filename, RAMDISK_ADDR);

            exit(1);

        }

    }



    

    if (kernel_filename) {

        if (bamboo_load_device_tree(FDT_ADDR, ram_size, RAMDISK_ADDR,

                                    initrd_size, kernel_cmdline) < 0) {

            fprintf(stderr, "couldn't load device tree\n");

            exit(1);

        }



        cpu_synchronize_state(env);



        

        env->gpr[1] = (16<<20) - 8;

        env->gpr[3] = FDT_ADDR;

        env->nip = entry;

        

    }



    if (kvm_enabled())

        kvmppc_init();

}