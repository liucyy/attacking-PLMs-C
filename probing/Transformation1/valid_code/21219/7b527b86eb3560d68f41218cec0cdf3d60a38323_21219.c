static int s390_ipl_init(SysBusDevice *dev)

{

    S390IPLState *ipl = S390_IPL(dev);

    uint64_t pentry = KERN_IMAGE_START;

    int kernel_size;



    int bios_size;

    char *bios_filename;



    

    if (!ipl->kernel || ipl->enforce_bios) {

        uint64_t fwbase = (MIN(ram_size, 0x80000000U) - 0x200000) & ~0xffffUL;



        if (bios_name == NULL) {

            bios_name = ipl->firmware;

        }



        bios_filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_name);

        if (bios_filename == NULL) {

            hw_error("could not find stage1 bootloader\n");

        }



        bios_size = load_elf(bios_filename, bios_translate_addr, &fwbase,

                             &ipl->bios_start_addr, NULL, NULL, 1,

                             ELF_MACHINE, 0);

        if (bios_size > 0) {

            

            ipl->bios_start_addr += fwbase;

        } else {

            

            bios_size = load_image_targphys(bios_filename, ZIPL_IMAGE_START,

                                            4096);

            ipl->bios_start_addr = ZIPL_IMAGE_START;

            if (bios_size > 4096) {

                hw_error("stage1 bootloader is > 4k\n");

            }

        }

        g_free(bios_filename);



        if (bios_size == -1) {

            hw_error("could not load bootloader '%s'\n", bios_name);

        }



        

        ipl->start_addr = ipl->bios_start_addr;

    }



    if (ipl->kernel) {

        kernel_size = load_elf(ipl->kernel, NULL, NULL, &pentry, NULL,

                               NULL, 1, ELF_MACHINE, 0);

        if (kernel_size < 0) {

            kernel_size = load_image_targphys(ipl->kernel, 0, ram_size);

        }

        if (kernel_size < 0) {

            fprintf(stderr, "could not load kernel '%s'\n", ipl->kernel);

            return -1;

        }

        

        if (pentry == KERN_IMAGE_START || pentry == 0x800) {

            ipl->start_addr = KERN_IMAGE_START;

            

            strcpy(rom_ptr(KERN_PARM_AREA), ipl->cmdline);

        } else {

            ipl->start_addr = pentry;

        }



        if (ipl->initrd) {

            ram_addr_t initrd_offset;

            int initrd_size;



            initrd_offset = INITRD_START;

            while (kernel_size + 0x100000 > initrd_offset) {

                initrd_offset += 0x100000;

            }

            initrd_size = load_image_targphys(ipl->initrd, initrd_offset,

                                              ram_size - initrd_offset);

            if (initrd_size == -1) {

                fprintf(stderr, "qemu: could not load initrd '%s'\n",

                        ipl->initrd);

                exit(1);

            }



            

            stq_p(rom_ptr(INITRD_PARM_START), initrd_offset);

            stq_p(rom_ptr(INITRD_PARM_SIZE), initrd_size);

        }

    }

    return 0;

}