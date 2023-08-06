static void ide_atapi_identify(IDEState *s)

{

    uint16_t *p;



    if (s->identify_set) {

	memcpy(s->io_buffer, s->identify_data, sizeof(s->identify_data));

	return;

    }



    memset(s->io_buffer, 0, 512);

    p = (uint16_t *)s->io_buffer;

    

    put_le16(p + 0, (2 << 14) | (5 << 8) | (1 << 7) | (2 << 5) | (0 << 0));

    padstr((char *)(p + 10), s->drive_serial_str, 20); 

    put_le16(p + 20, 3); 

    put_le16(p + 21, 512); 

    put_le16(p + 22, 4); 

    padstr((char *)(p + 23), s->version, 8); 

    padstr((char *)(p + 27), "QEMU DVD-ROM", 40); 

    put_le16(p + 48, 1); 

#ifdef USE_DMA_CDROM

    put_le16(p + 49, 1 << 9 | 1 << 8); 

    put_le16(p + 53, 7); 

    put_le16(p + 62, 7);  

    put_le16(p + 63, 7);  

#else

    put_le16(p + 49, 1 << 9); 

    put_le16(p + 53, 3); 

    put_le16(p + 63, 0x103); 

#endif

    put_le16(p + 64, 3); 

    put_le16(p + 65, 0xb4); 

    put_le16(p + 66, 0xb4); 

    put_le16(p + 67, 0x12c); 

    put_le16(p + 68, 0xb4); 



    put_le16(p + 71, 30); 

    put_le16(p + 72, 30); 



    if (s->ncq_queues) {

        put_le16(p + 75, s->ncq_queues - 1);

        

        put_le16(p + 76, (1 << 8));

    }



    put_le16(p + 80, 0x1e); 

#ifdef USE_DMA_CDROM

    put_le16(p + 88, 0x3f | (1 << 13)); 

#endif

    memcpy(s->identify_data, p, sizeof(s->identify_data));

    s->identify_set = 1;

}