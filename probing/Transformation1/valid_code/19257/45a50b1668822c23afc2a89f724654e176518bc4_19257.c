static void generate_bootsect(target_phys_addr_t option_rom,

                              uint32_t gpr[8], uint16_t segs[6], uint16_t ip)

{

    uint8_t rom[512], *p, *reloc;

    uint8_t sum;

    int i;



    memset(rom, 0, sizeof(rom));



    p = rom;

    

    *p++ = 0x55;

    *p++ = 0xaa;



    

    *p++ = 1;



    



    *p++ = 0x50;		

    *p++ = 0x1e;		

    *p++ = 0x31; *p++ = 0xc0;	

    *p++ = 0x8e; *p++ = 0xd8;	



    *p++ = 0xc7; *p++ = 0x06;   

    *p++ = 0x64; *p++ = 0x00;

    reloc = p;

    *p++ = 0x00; *p++ = 0x00;



    *p++ = 0x8c; *p++ = 0x0e;   

    *p++ = 0x66; *p++ = 0x00;



    *p++ = 0x1f;		

    *p++ = 0x58;		

    *p++ = 0xcb;		



    

    *reloc = (p - rom);



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



    

    sum = 0;

    for (i = 0; i < (sizeof(rom) - 1); i++)

        sum += rom[i];

    rom[sizeof(rom) - 1] = -sum;



    cpu_physical_memory_write_rom(option_rom, rom, sizeof(rom));

    option_rom_setup_reset(option_rom, sizeof (rom));

}