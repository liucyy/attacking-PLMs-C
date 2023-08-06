void pc_cmos_init(ram_addr_t ram_size, ram_addr_t above_4g_mem_size,

                  const char *boot_device,

                  BusState *idebus0, BusState *idebus1,

                  FDCtrl *floppy_controller, ISADevice *s)

{

    int val, nb;

    FDriveType fd0, fd1;

    static pc_cmos_init_late_arg arg;



    



    

    val = 640; 

    rtc_set_memory(s, 0x15, val);

    rtc_set_memory(s, 0x16, val >> 8);



    val = (ram_size / 1024) - 1024;

    if (val > 65535)

        val = 65535;

    rtc_set_memory(s, 0x17, val);

    rtc_set_memory(s, 0x18, val >> 8);

    rtc_set_memory(s, 0x30, val);

    rtc_set_memory(s, 0x31, val >> 8);



    if (above_4g_mem_size) {

        rtc_set_memory(s, 0x5b, (unsigned int)above_4g_mem_size >> 16);

        rtc_set_memory(s, 0x5c, (unsigned int)above_4g_mem_size >> 24);

        rtc_set_memory(s, 0x5d, (uint64_t)above_4g_mem_size >> 32);

    }



    if (ram_size > (16 * 1024 * 1024))

        val = (ram_size / 65536) - ((16 * 1024 * 1024) / 65536);

    else

        val = 0;

    if (val > 65535)

        val = 65535;

    rtc_set_memory(s, 0x34, val);

    rtc_set_memory(s, 0x35, val >> 8);



    

    rtc_set_memory(s, 0x5f, smp_cpus - 1);



    

    if (set_boot_dev(s, boot_device, fd_bootchk)) {

        exit(1);

    }



    



    fd0 = fdctrl_get_drive_type(floppy_controller, 0);

    fd1 = fdctrl_get_drive_type(floppy_controller, 1);



    val = (cmos_get_fd_drive_type(fd0) << 4) | cmos_get_fd_drive_type(fd1);

    rtc_set_memory(s, 0x10, val);



    val = 0;

    nb = 0;

    if (fd0 < FDRIVE_DRV_NONE) {

        nb++;

    }

    if (fd1 < FDRIVE_DRV_NONE) {

        nb++;

    }

    switch (nb) {

    case 0:

        break;

    case 1:

        val |= 0x01; 

        break;

    case 2:

        val |= 0x41; 

        break;

    }

    val |= 0x02; 

    val |= 0x04; 

    rtc_set_memory(s, REG_EQUIPMENT_BYTE, val);



    

    arg.rtc_state = s;

    arg.idebus0 = idebus0;

    arg.idebus1 = idebus1;

    qemu_register_reset(pc_cmos_init_late, &arg);

}