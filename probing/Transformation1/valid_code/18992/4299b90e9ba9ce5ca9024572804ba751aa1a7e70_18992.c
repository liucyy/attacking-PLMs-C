static void cirrus_do_copy(CirrusVGAState *s, int dst, int src, int w, int h)

{

    int sx = 0, sy = 0;

    int dx = 0, dy = 0;

    int depth = 0;

    int notify = 0;



    

    if (*s->cirrus_rop == cirrus_bitblt_rop_fwd_src ||

        *s->cirrus_rop == cirrus_bitblt_rop_bkwd_src) {



        int width, height;



        depth = s->vga.get_bpp(&s->vga) / 8;

        s->vga.get_resolution(&s->vga, &width, &height);



        

        sx = (src % ABS(s->cirrus_blt_srcpitch)) / depth;

        sy = (src / ABS(s->cirrus_blt_srcpitch));

        dx = (dst % ABS(s->cirrus_blt_dstpitch)) / depth;

        dy = (dst / ABS(s->cirrus_blt_dstpitch));



        

        w /= depth;



        

        if (s->cirrus_blt_dstpitch < 0) {

            sx -= (s->cirrus_blt_width / depth) - 1;

            dx -= (s->cirrus_blt_width / depth) - 1;

            sy -= s->cirrus_blt_height - 1;

            dy -= s->cirrus_blt_height - 1;

        }



        

        if (sx >= 0 && sy >= 0 && dx >= 0 && dy >= 0 &&

            (sx + w) <= width && (sy + h) <= height &&

            (dx + w) <= width && (dy + h) <= height) {

            notify = 1;

        }

    }



    

    if (notify)

        graphic_hw_update(s->vga.con);



    (*s->cirrus_rop) (s, s->vga.vram_ptr +

		      (s->cirrus_blt_dstaddr & s->cirrus_addr_mask),

		      s->vga.vram_ptr +

		      (s->cirrus_blt_srcaddr & s->cirrus_addr_mask),

		      s->cirrus_blt_dstpitch, s->cirrus_blt_srcpitch,

		      s->cirrus_blt_width, s->cirrus_blt_height);



    if (notify) {

        qemu_console_copy(s->vga.con,

			  sx, sy, dx, dy,

			  s->cirrus_blt_width / depth,

			  s->cirrus_blt_height);

    }



    



    cirrus_invalidate_region(s, s->cirrus_blt_dstaddr,

				s->cirrus_blt_dstpitch, s->cirrus_blt_width,

				s->cirrus_blt_height);

}