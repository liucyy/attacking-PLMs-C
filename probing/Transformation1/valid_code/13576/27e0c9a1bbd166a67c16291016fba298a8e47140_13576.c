static void ide_cfata_identify(IDEState *s)

{

    uint16_t *p;

    uint32_t cur_sec;



    p = (uint16_t *) s->identify_data;

    if (s->identify_set)

        goto fill_buffer;



    memset(p, 0, sizeof(s->identify_data));



    cur_sec = s->cylinders * s->heads * s->sectors;



    put_le16(p + 0, 0x848a);			

    put_le16(p + 1, s->cylinders);		

    put_le16(p + 3, s->heads);			

    put_le16(p + 6, s->sectors);		

    put_le16(p + 7, s->nb_sectors >> 16);	

    put_le16(p + 8, s->nb_sectors);		

    padstr((char *)(p + 10), s->drive_serial_str, 20); 

    put_le16(p + 22, 0x0004);			

    padstr((char *) (p + 23), s->version, 8);	

    padstr((char *) (p + 27), "QEMU MICRODRIVE", 40);

#if MAX_MULT_SECTORS > 1

    put_le16(p + 47, 0x8000 | MAX_MULT_SECTORS);

#else

    put_le16(p + 47, 0x0000);

#endif

    put_le16(p + 49, 0x0f00);			

    put_le16(p + 51, 0x0002);			

    put_le16(p + 52, 0x0001);			

    put_le16(p + 53, 0x0003);			

    put_le16(p + 54, s->cylinders);		

    put_le16(p + 55, s->heads);			

    put_le16(p + 56, s->sectors);		

    put_le16(p + 57, cur_sec);			

    put_le16(p + 58, cur_sec >> 16);		

    if (s->mult_sectors)			

        put_le16(p + 59, 0x100 | s->mult_sectors);

    put_le16(p + 60, s->nb_sectors);		

    put_le16(p + 61, s->nb_sectors >> 16);	

    put_le16(p + 63, 0x0203);			

    put_le16(p + 64, 0x0001);			

    put_le16(p + 65, 0x0096);			

    put_le16(p + 66, 0x0096);			

    put_le16(p + 68, 0x00b4);			

    put_le16(p + 82, 0x400c);			

    put_le16(p + 83, 0x7068);			

    put_le16(p + 84, 0x4000);			

    put_le16(p + 85, 0x000c);			

    put_le16(p + 86, 0x7044);			

    put_le16(p + 87, 0x4000);			

    put_le16(p + 91, 0x4060);			

    put_le16(p + 129, 0x0002);			

    put_le16(p + 130, 0x0005);			

    put_le16(p + 131, 0x0001);			

    put_le16(p + 132, 0x0000);			

    put_le16(p + 160, 0x8100);			

    put_le16(p + 161, 0x8001);			



    s->identify_set = 1;



fill_buffer:

    memcpy(s->io_buffer, p, sizeof(s->identify_data));

}