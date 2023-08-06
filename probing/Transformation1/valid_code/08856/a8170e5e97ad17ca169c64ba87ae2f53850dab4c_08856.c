static uint64_t cirrus_linear_read(void *opaque, target_phys_addr_t addr,

                                   unsigned size)

{

    CirrusVGAState *s = opaque;

    uint32_t ret;



    addr &= s->cirrus_addr_mask;



    if (((s->vga.sr[0x17] & 0x44) == 0x44) &&

        ((addr & s->linear_mmio_mask) == s->linear_mmio_mask)) {

	

	ret = cirrus_mmio_blt_read(s, addr & 0xff);

    } else if (0) {

	

	ret = 0xff;

    } else {

	

	if ((s->vga.gr[0x0B] & 0x14) == 0x14) {

	    addr <<= 4;

	} else if (s->vga.gr[0x0B] & 0x02) {

	    addr <<= 3;

	}

	addr &= s->cirrus_addr_mask;

	ret = *(s->vga.vram_ptr + addr);

    }



    return ret;

}