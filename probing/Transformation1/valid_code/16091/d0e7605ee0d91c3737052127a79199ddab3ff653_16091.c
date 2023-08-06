pflash_t *pflash_cfi01_register(target_phys_addr_t base, ram_addr_t off,

                                BlockDriverState *bs, uint32_t sector_len,

                                int nb_blocs, int width,

                                uint16_t id0, uint16_t id1,

                                uint16_t id2, uint16_t id3)

{

    pflash_t *pfl;

    target_phys_addr_t total_len;



    total_len = sector_len * nb_blocs;



    

#if 0

    if (total_len != (8 * 1024 * 1024) && total_len != (16 * 1024 * 1024) &&

        total_len != (32 * 1024 * 1024) && total_len != (64 * 1024 * 1024))

        return NULL;

#endif



    pfl = qemu_mallocz(sizeof(pflash_t));



    

    pfl->storage = qemu_get_ram_ptr(off);

    pfl->fl_mem = cpu_register_io_memory(

                    pflash_read_ops, pflash_write_ops, pfl);

    pfl->off = off;

    cpu_register_physical_memory(base, total_len,

                    off | pfl->fl_mem | IO_MEM_ROMD);



    pfl->bs = bs;

    if (pfl->bs) {

        

        bdrv_read(pfl->bs, 0, pfl->storage, total_len >> 9);

    }

#if 0 

    pfl->ro = 1;

#else

    pfl->ro = 0;

#endif

    pfl->timer = qemu_new_timer(vm_clock, pflash_timer, pfl);

    pfl->base = base;

    pfl->sector_len = sector_len;

    pfl->total_len = total_len;

    pfl->width = width;

    pfl->wcycle = 0;

    pfl->cmd = 0;

    pfl->status = 0;

    pfl->ident[0] = id0;

    pfl->ident[1] = id1;

    pfl->ident[2] = id2;

    pfl->ident[3] = id3;

    

    pfl->cfi_len = 0x52;

    

    pfl->cfi_table[0x10] = 'Q';

    pfl->cfi_table[0x11] = 'R';

    pfl->cfi_table[0x12] = 'Y';

    

    pfl->cfi_table[0x13] = 0x01;

    pfl->cfi_table[0x14] = 0x00;

    

    pfl->cfi_table[0x15] = 0x31;

    pfl->cfi_table[0x16] = 0x00;

    

    pfl->cfi_table[0x17] = 0x00;

    pfl->cfi_table[0x18] = 0x00;

    

    pfl->cfi_table[0x19] = 0x00;

    pfl->cfi_table[0x1A] = 0x00;

    

    pfl->cfi_table[0x1B] = 0x45;

    

    pfl->cfi_table[0x1C] = 0x55;

    

    pfl->cfi_table[0x1D] = 0x00;

    

    pfl->cfi_table[0x1E] = 0x00;

    

    pfl->cfi_table[0x1F] = 0x07;

    

    pfl->cfi_table[0x20] = 0x07;

    

    pfl->cfi_table[0x21] = 0x0a;

    

    pfl->cfi_table[0x22] = 0x00;

    

    pfl->cfi_table[0x23] = 0x04;

    

    pfl->cfi_table[0x24] = 0x04;

    

    pfl->cfi_table[0x25] = 0x04;

    

    pfl->cfi_table[0x26] = 0x00;

    

    pfl->cfi_table[0x27] = ctz32(total_len); 

    

    pfl->cfi_table[0x28] = 0x02;

    pfl->cfi_table[0x29] = 0x00;

    

    pfl->cfi_table[0x2A] = 0x0B;

    pfl->cfi_table[0x2B] = 0x00;

    

    pfl->cfi_table[0x2C] = 0x01;

    

    pfl->cfi_table[0x2D] = nb_blocs - 1;

    pfl->cfi_table[0x2E] = (nb_blocs - 1) >> 8;

    pfl->cfi_table[0x2F] = sector_len >> 8;

    pfl->cfi_table[0x30] = sector_len >> 16;



    

    pfl->cfi_table[0x31] = 'P';

    pfl->cfi_table[0x32] = 'R';

    pfl->cfi_table[0x33] = 'I';



    pfl->cfi_table[0x34] = '1';

    pfl->cfi_table[0x35] = '1';



    pfl->cfi_table[0x36] = 0x00;

    pfl->cfi_table[0x37] = 0x00;

    pfl->cfi_table[0x38] = 0x00;

    pfl->cfi_table[0x39] = 0x00;



    pfl->cfi_table[0x3a] = 0x00;



    pfl->cfi_table[0x3b] = 0x00;

    pfl->cfi_table[0x3c] = 0x00;



    return pfl;

}