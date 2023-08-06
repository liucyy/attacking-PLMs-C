static uint16_t blizzard_reg_read(void *opaque, uint8_t reg)

{

    BlizzardState *s = (BlizzardState *) opaque;



    switch (reg) {

    case 0x00:	

        return 0xa5;



    case 0x02:	

        return 0x83;	



    case 0x04:	

        return (s->pll - 1) | (1 << 7);

    case 0x06:	

        return s->pll_range;

    case 0x08:	

        return s->pll_ctrl & 0xff;

    case 0x0a:	

        return s->pll_ctrl >> 8;

    case 0x0c:	

        return s->pll_mode;



    case 0x0e:	

        return s->clksel;



    case 0x10:	

    case 0x14:	

        return s->memenable;



    case 0x18:	

        return s->memrefresh & 0xff;

    case 0x1a:	

        return s->memrefresh >> 8;



    case 0x1c:	

        return s->timing[0];

    case 0x1e:	

        return s->timing[1];

    case 0x20:	

        return s->timing[2];



    case 0x24:	

        return s->priority;



    case 0x28:	

        return s->lcd_config;



    case 0x2a:	

        return s->x >> 3;

    case 0x2c:	

        return s->hndp;

    case 0x2e:	

        return s->y & 0xff;

    case 0x30:	

        return s->y >> 8;

    case 0x32:	

        return s->vndp;

    case 0x34:	

        return s->hsync;

    case 0x36:	

        return s->skipx >> 3;

    case 0x38:	

        return s->vsync;

    case 0x3a:	

        return s->skipy;



    case 0x3c:	

        return s->pclk;



    case 0x3e:	

        return s->hssi_config[0];

    case 0x40:	

        return s->hssi_config[1];

    case 0x42:	

        return s->hssi_config[2];

    case 0x44:	

        return s->tv_config;

    case 0x46 ... 0x4c:	

        return s->tv_timing[(reg - 0x46) >> 1];

    case 0x4e:	

        return s->vbi;

    case 0x50:	

        return s->tv_x;

    case 0x52:	

        return s->tv_y;

    case 0x54:	

        return s->tv_test;

    case 0x56:	

        return s->tv_filter_config;

    case 0x58:	

        return s->tv_filter_idx;

    case 0x5a:	

        if (s->tv_filter_idx < 0x20)

            return s->tv_filter_coeff[s->tv_filter_idx ++];

        return 0;



    case 0x60:	

        return s->yrc[0];

    case 0x62:	

        return s->yrc[1];

    case 0x64:	

        return s->u;

    case 0x66:	

        return s->v;



    case 0x68:	

        return s->mode;



    case 0x6a:	

        return s->effect;



    case 0x6c:	

        return s->ix[0] & 0xff;

    case 0x6e:	

        return s->ix[0] >> 3;

    case 0x70:	

        return s->ix[0] & 0xff;

    case 0x72:	

        return s->ix[0] >> 3;

    case 0x74:	

        return s->ix[1] & 0xff;

    case 0x76:	

        return s->ix[1] >> 3;

    case 0x78:	

        return s->ix[1] & 0xff;

    case 0x7a:	

        return s->ix[1] >> 3;

    case 0x7c:	

        return s->ox[0] & 0xff;

    case 0x7e:	

        return s->ox[0] >> 3;

    case 0x80:	

        return s->oy[0] & 0xff;

    case 0x82:	

        return s->oy[0] >> 3;

    case 0x84:	

        return s->ox[1] & 0xff;

    case 0x86:	

        return s->ox[1] >> 3;

    case 0x88:	

        return s->oy[1] & 0xff;

    case 0x8a:	

        return s->oy[1] >> 3;



    case 0x8c:	

        return s->iformat;

    case 0x8e:	

        return s->source;

    case 0x90:	

        return 0;



    case 0xa8:	

        return s->border_r;

    case 0xaa:	

        return s->border_g;

    case 0xac:	

        return s->border_b;



    case 0xb4:	

        return s->gamma_config;

    case 0xb6:	

        return s->gamma_idx;

    case 0xb8:	

        return s->gamma_lut[s->gamma_idx ++];



    case 0xba:	

        return s->matrix_ena;

    case 0xbc ... 0xde:	

        return s->matrix_coeff[(reg - 0xbc) >> 1];

    case 0xe0:	

        return s->matrix_r;

    case 0xe2:	

        return s->matrix_g;

    case 0xe4:	

        return s->matrix_b;



    case 0xe6:	

        return s->pm;

    case 0xe8:	

        return s->status | (1 << 5);

    case 0xea:	

        return s->rgbgpio_dir;

    case 0xec:	

        return s->rgbgpio;

    case 0xee:	

        return s->gpio_dir;

    case 0xf0:	

        return s->gpio;

    case 0xf2:	

        return s->gpio_edge[0];

    case 0xf4:	

        return s->gpio_edge[1];

    case 0xf6:	

        return s->gpio_irq;

    case 0xf8:	

        return s->gpio_pdown;



    default:

        fprintf(stderr, "%s: unknown register %02x\n", __FUNCTION__, reg);

        return 0;

    }

}