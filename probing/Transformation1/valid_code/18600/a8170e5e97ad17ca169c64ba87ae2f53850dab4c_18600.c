static void g364fb_ctrl_write(void *opaque,

                              target_phys_addr_t addr,

                              uint64_t val,

                              unsigned int size)

{

    G364State *s = opaque;



    trace_g364fb_write(addr, val);



    if (addr >= REG_CLR_PAL && addr < REG_CLR_PAL + 0x800) {

        

        int idx = (addr - REG_CLR_PAL) >> 3;

        s->color_palette[idx][0] = (val >> 16) & 0xff;

        s->color_palette[idx][1] = (val >> 8) & 0xff;

        s->color_palette[idx][2] = val & 0xff;

        g364fb_invalidate_display(s);

    } else if (addr >= REG_CURS_PAT && addr < REG_CURS_PAT + 0x1000) {

        

        int idx = (addr - REG_CURS_PAT) >> 3;

        s->cursor[idx] = val;

        g364fb_invalidate_display(s);

    } else if (addr >= REG_CURS_PAL && addr < REG_CURS_PAL + 0x18) {

        

        int idx = (addr - REG_CURS_PAL) >> 3;

        s->cursor_palette[idx][0] = (val >> 16) & 0xff;

        s->cursor_palette[idx][1] = (val >> 8) & 0xff;

        s->cursor_palette[idx][2] = val & 0xff;

        g364fb_invalidate_display(s);

    } else {

        switch (addr) {

        case REG_BOOT: 

        case 0x00108: 

        case 0x00110: 

        case 0x00120: 

        case 0x00128: 

        case 0x00130: 

        case 0x00138: 

        case 0x00140: 

        case 0x00148: 

        case 0x00158: 

        case 0x00160: 

        case 0x00168: 

        case 0x00170: 

        case 0x00200: 

            

            break;

        case REG_TOP:

            s->top_of_screen = val;

            g364fb_invalidate_display(s);

            break;

        case REG_DISPLAY:

            s->width = val * 4;

            break;

        case REG_VDISPLAY:

            s->height = val / 2;

            break;

        case REG_CTLA:

            s->ctla = val;

            g364fb_update_depth(s);

            g364fb_invalidate_display(s);

            break;

        case REG_CURS_POS:

            g364_invalidate_cursor_position(s);

            s->cursor_position = val;

            g364_invalidate_cursor_position(s);

            break;

        case REG_RESET:

            g364fb_reset(s);

            break;

        default:

            error_report("g364: invalid write of 0x%" PRIx64

                         " at [" TARGET_FMT_plx "]", val, addr);

            break;

        }

    }

    qemu_irq_lower(s->irq);

}