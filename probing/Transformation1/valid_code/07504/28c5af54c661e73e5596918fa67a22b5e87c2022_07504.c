static void cmos_init(int ram_size, const char *boot_device, BlockDriverState **hd_table)

{

    RTCState *s = rtc_state;

    int val;

    int fd0, fd1, nb;

    int i;



    



    

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



    if (ram_size > (16 * 1024 * 1024))

        val = (ram_size / 65536) - ((16 * 1024 * 1024) / 65536);

    else

        val = 0;

    if (val > 65535)

        val = 65535;

    rtc_set_memory(s, 0x34, val);

    rtc_set_memory(s, 0x35, val >> 8);



    

    rtc_set_memory(s, 0x3d,

            boot_device2nibble(boot_device[1]) << 4 |

            boot_device2nibble(boot_device[0]) );

    rtc_set_memory(s, 0x38,

            boot_device2nibble(boot_device[2]) << 4 | (fd_bootchk ?  0x0 : 0x1));



    



    fd0 = fdctrl_get_drive_type(floppy_controller, 0);

    fd1 = fdctrl_get_drive_type(floppy_controller, 1);



    val = (cmos_get_fd_drive_type(fd0) << 4) | cmos_get_fd_drive_type(fd1);

    rtc_set_memory(s, 0x10, val);



    val = 0;

    nb = 0;

    if (fd0 < 3)

        nb++;

    if (fd1 < 3)

        nb++;

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



    



    rtc_set_memory(s, 0x12, (hd_table[0] ? 0xf0 : 0) | (hd_table[1] ? 0x0f : 0));

    if (hd_table[0])

        cmos_init_hd(0x19, 0x1b, hd_table[0]);

    if (hd_table[1])

        cmos_init_hd(0x1a, 0x24, hd_table[1]);



    val = 0;

    for (i = 0; i < 4; i++) {

        if (hd_table[i]) {

            int cylinders, heads, sectors, translation;

            

            translation = bdrv_get_translation_hint(hd_table[i]);

            if (translation == BIOS_ATA_TRANSLATION_AUTO) {

                bdrv_get_geometry_hint(hd_table[i], &cylinders, &heads, &sectors);

                if (cylinders <= 1024 && heads <= 16 && sectors <= 63) {

                    

                    translation = 0;

                } else {

                    

                    translation = 1;

                }

            } else {

                translation--;

            }

            val |= translation << (i * 2);

        }

    }

    rtc_set_memory(s, 0x39, val);

}