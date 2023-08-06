void vbe_ioport_write_data(void *opaque, uint32_t addr, uint32_t val)

{

    VGACommonState *s = opaque;



    if (s->vbe_index <= VBE_DISPI_INDEX_NB) {

#ifdef DEBUG_BOCHS_VBE

        printf("VBE: write index=0x%x val=0x%x\n", s->vbe_index, val);

#endif

        switch(s->vbe_index) {

        case VBE_DISPI_INDEX_ID:

            if (val == VBE_DISPI_ID0 ||

                val == VBE_DISPI_ID1 ||

                val == VBE_DISPI_ID2 ||

                val == VBE_DISPI_ID3 ||

                val == VBE_DISPI_ID4) {

                s->vbe_regs[s->vbe_index] = val;

            }

            break;

        case VBE_DISPI_INDEX_XRES:

        case VBE_DISPI_INDEX_YRES:

        case VBE_DISPI_INDEX_BPP:

        case VBE_DISPI_INDEX_VIRT_WIDTH:

        case VBE_DISPI_INDEX_X_OFFSET:

        case VBE_DISPI_INDEX_Y_OFFSET:

            s->vbe_regs[s->vbe_index] = val;

            vbe_fixup_regs(s);

            break;

        case VBE_DISPI_INDEX_BANK:

            if (s->vbe_regs[VBE_DISPI_INDEX_BPP] == 4) {

              val &= (s->vbe_bank_mask >> 2);

            } else {

              val &= s->vbe_bank_mask;

            }

            s->vbe_regs[s->vbe_index] = val;

            s->bank_offset = (val << 16);

            vga_update_memory_access(s);

            break;

        case VBE_DISPI_INDEX_ENABLE:

            if ((val & VBE_DISPI_ENABLED) &&

                !(s->vbe_regs[VBE_DISPI_INDEX_ENABLE] & VBE_DISPI_ENABLED)) {

                int h, shift_control;



                s->vbe_regs[VBE_DISPI_INDEX_VIRT_WIDTH] = 0;

                s->vbe_regs[VBE_DISPI_INDEX_X_OFFSET] = 0;

                s->vbe_regs[VBE_DISPI_INDEX_Y_OFFSET] = 0;

                s->vbe_regs[VBE_DISPI_INDEX_ENABLE] |= VBE_DISPI_ENABLED;

                vbe_fixup_regs(s);



                

                if (!(val & VBE_DISPI_NOCLEARMEM)) {

                    memset(s->vram_ptr, 0,

                           s->vbe_regs[VBE_DISPI_INDEX_YRES] * s->vbe_line_offset);

                }



                

                

                s->gr[VGA_GFX_MISC] = (s->gr[VGA_GFX_MISC] & ~0x0c) | 0x04 |

                    VGA_GR06_GRAPHICS_MODE;

                s->cr[VGA_CRTC_MODE] |= 3; 

                s->cr[VGA_CRTC_OFFSET] = s->vbe_line_offset >> 3;

                

                s->cr[VGA_CRTC_H_DISP] =

                    (s->vbe_regs[VBE_DISPI_INDEX_XRES] >> 3) - 1;

                

                h = s->vbe_regs[VBE_DISPI_INDEX_YRES] - 1;

                s->cr[VGA_CRTC_V_DISP_END] = h;

                s->cr[VGA_CRTC_OVERFLOW] = (s->cr[VGA_CRTC_OVERFLOW] & ~0x42) |

                    ((h >> 7) & 0x02) | ((h >> 3) & 0x40);

                

                s->cr[VGA_CRTC_LINE_COMPARE] = 0xff;

                s->cr[VGA_CRTC_OVERFLOW] |= 0x10;

                s->cr[VGA_CRTC_MAX_SCAN] |= 0x40;



                if (s->vbe_regs[VBE_DISPI_INDEX_BPP] == 4) {

                    shift_control = 0;

                    s->sr[VGA_SEQ_CLOCK_MODE] &= ~8; 

                } else {

                    shift_control = 2;

                    

                    s->sr[VGA_SEQ_MEMORY_MODE] |= VGA_SR04_CHN_4M;

                    

                    s->sr[VGA_SEQ_PLANE_WRITE] |= VGA_SR02_ALL_PLANES;

                }

                s->gr[VGA_GFX_MODE] = (s->gr[VGA_GFX_MODE] & ~0x60) |

                    (shift_control << 5);

                s->cr[VGA_CRTC_MAX_SCAN] &= ~0x9f; 

            } else {

                s->bank_offset = 0;

            }

            s->dac_8bit = (val & VBE_DISPI_8BIT_DAC) > 0;

            s->vbe_regs[s->vbe_index] = val;

            vga_update_memory_access(s);

            break;

        default:

            break;

        }

    }

}