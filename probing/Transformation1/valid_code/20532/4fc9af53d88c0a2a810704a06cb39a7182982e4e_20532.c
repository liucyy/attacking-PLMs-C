static void generate_bootsect(uint32_t gpr[8], uint16_t segs[6], uint16_t ip)

{

    uint8_t bootsect[512], *p;

    int i;

    int hda;



    hda = drive_get_index(IF_IDE, 0, 0);

    if (hda == -1) {

	fprintf(stderr, "A disk image must be given for 'hda' when booting "

		"a Linux kernel\n(if you really don't want it, use /dev/zero)\n");

	exit(1);

    }



    memset(bootsect, 0, sizeof(bootsect));



    

    bdrv_read(drives_table[hda].bdrv, 0, bootsect, 1);



    

    bootsect[510] = 0x55;

    bootsect[511] = 0xaa;



    

    p = bootsect;

    *p++ = 0xfa;		

    *p++ = 0xfc;		



    for (i = 0; i < 6; i++) {

	if (i == 1)		

	    continue;



	*p++ = 0xb8;		

	*p++ = segs[i];

	*p++ = segs[i] >> 8;

	*p++ = 0x8e;		

	*p++ = 0xc0 + (i << 3);

    }



    for (i = 0; i < 8; i++) {

	*p++ = 0x66;		

	*p++ = 0xb8 + i;	

	*p++ = gpr[i];

	*p++ = gpr[i] >> 8;

	*p++ = gpr[i] >> 16;

	*p++ = gpr[i] >> 24;

    }



    *p++ = 0xea;		

    *p++ = ip;			

    *p++ = ip >> 8;

    *p++ = segs[1];		

    *p++ = segs[1] >> 8;



    bdrv_set_boot_sector(drives_table[hda].bdrv, bootsect, sizeof(bootsect));

}