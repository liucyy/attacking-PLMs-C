static void pl110_update_display(void *opaque)

{

    PL110State *s = (PL110State *)opaque;

    SysBusDevice *sbd;

    DisplaySurface *surface = qemu_console_surface(s->con);

    drawfn* fntable;

    drawfn fn;

    int dest_width;

    int src_width;

    int bpp_offset;

    int first;

    int last;



    if (!pl110_enabled(s)) {

        return;

    }



    sbd = SYS_BUS_DEVICE(s);



    switch (surface_bits_per_pixel(surface)) {

    case 0:

        return;

    case 8:

        fntable = pl110_draw_fn_8;

        dest_width = 1;

        break;

    case 15:

        fntable = pl110_draw_fn_15;

        dest_width = 2;

        break;

    case 16:

        fntable = pl110_draw_fn_16;

        dest_width = 2;

        break;

    case 24:

        fntable = pl110_draw_fn_24;

        dest_width = 3;

        break;

    case 32:

        fntable = pl110_draw_fn_32;

        dest_width = 4;

        break;

    default:

        fprintf(stderr, "pl110: Bad color depth\n");

        exit(1);

    }

    if (s->cr & PL110_CR_BGR)

        bpp_offset = 0;

    else

        bpp_offset = 24;



    if ((s->version != PL111) && (s->bpp == BPP_16)) {

        

        switch (s->mux_ctrl) {

        case 3: 

            bpp_offset = (BPP_16_565 - BPP_16);

            break;

        case 1: 

            break;

        case 0: 

        case 2: 

        default:

            

            bpp_offset += (BPP_16_565 - BPP_16);

            break;

        }

    }



    if (s->cr & PL110_CR_BEBO)

        fn = fntable[s->bpp + 8 + bpp_offset];

    else if (s->cr & PL110_CR_BEPO)

        fn = fntable[s->bpp + 16 + bpp_offset];

    else

        fn = fntable[s->bpp + bpp_offset];



    src_width = s->cols;

    switch (s->bpp) {

    case BPP_1:

        src_width >>= 3;

        break;

    case BPP_2:

        src_width >>= 2;

        break;

    case BPP_4:

        src_width >>= 1;

        break;

    case BPP_8:

        break;

    case BPP_16:

    case BPP_16_565:

    case BPP_12:

        src_width <<= 1;

        break;

    case BPP_32:

        src_width <<= 2;

        break;

    }

    dest_width *= s->cols;

    first = 0;

    framebuffer_update_display(surface, sysbus_address_space(sbd),

                               s->upbase, s->cols, s->rows,

                               src_width, dest_width, 0,

                               s->invalidate,

                               fn, s->palette,

                               &first, &last);

    if (first >= 0) {

        dpy_gfx_update(s->con, 0, first, s->cols, last - first + 1);

    }

    s->invalidate = 0;

}