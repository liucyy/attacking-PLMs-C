static void arm_sysctl_write(void *opaque, target_phys_addr_t offset,

                             uint64_t val, unsigned size)

{

    arm_sysctl_state *s = (arm_sysctl_state *)opaque;



    switch (offset) {

    case 0x08: 

        s->leds = val;

    case 0x0c: 

    case 0x10: 

    case 0x14: 

    case 0x18: 

    case 0x1c: 

        

        break;

    case 0x20: 

        if (val == LOCK_VALUE)

            s->lockval = val;

        else

            s->lockval = val & 0x7fff;

        break;

    case 0x28: 

        

        s->cfgdata1 = val;

        break;

    case 0x2c: 

        

        s->cfgdata2 = val;

        break;

    case 0x30: 

        s->flags |= val;

        break;

    case 0x34: 

        s->flags &= ~val;

        break;

    case 0x38: 

        s->nvflags |= val;

        break;

    case 0x3c: 

        s->nvflags &= ~val;

        break;

    case 0x40: 

        switch (board_id(s)) {

        case BOARD_ID_PB926:

            if (s->lockval == LOCK_VALUE) {

                s->resetlevel = val;

                if (val & 0x100) {

                    qemu_system_reset_request();

                }

            }

            break;

        case BOARD_ID_PBX:

        case BOARD_ID_PBA8:

            if (s->lockval == LOCK_VALUE) {

                s->resetlevel = val;

                if (val & 0x04) {

                    qemu_system_reset_request();

                }

            }

            break;

        case BOARD_ID_VEXPRESS:

        case BOARD_ID_EB:

        default:

            

            break;

        }

        break;

    case 0x44: 

        

        break;

    case 0x4c: 

        break;

    case 0x50: 

        switch (board_id(s)) {

        case BOARD_ID_PB926:

            

            s->sys_clcd &= 0x3f00;

            s->sys_clcd |= val & ~0x3f00;

            qemu_set_irq(s->pl110_mux_ctrl, val & 3);

            break;

        case BOARD_ID_EB:

            

            s->sys_clcd &= 0x3f00;

            s->sys_clcd |= val & ~0x3f00;

            break;

        case BOARD_ID_PBA8:

        case BOARD_ID_PBX:

            

            s->sys_clcd &= (1 << 7);

            s->sys_clcd |= val & ~(1 << 7);

            break;

        case BOARD_ID_VEXPRESS:

        default:

            

            break;

        }

    case 0x54: 

    case 0x64: 

    case 0x68: 

    case 0x6c: 

    case 0x70: 

    case 0x74: 

    case 0x80: 

    case 0x84: 

    case 0x88: 

    case 0x8c: 

    case 0x90: 

    case 0x94: 

    case 0x98: 

    case 0x9c: 

        break;

    case 0xa0: 

        if (board_id(s) != BOARD_ID_VEXPRESS) {

            goto bad_reg;

        }

        s->sys_cfgdata = val;

        return;

    case 0xa4: 

        if (board_id(s) != BOARD_ID_VEXPRESS) {

            goto bad_reg;

        }

        s->sys_cfgctrl = val & ~(3 << 18);

        s->sys_cfgstat = 1;            

        switch (s->sys_cfgctrl) {

        case 0xc0800000:            

            qemu_system_shutdown_request();

            break;

        case 0xc0900000:            

            qemu_system_reset_request();

            break;

        default:

            s->sys_cfgstat |= 2;        

        }

        return;

    case 0xa8: 

        if (board_id(s) != BOARD_ID_VEXPRESS) {

            goto bad_reg;

        }

        s->sys_cfgstat = val & 3;

        return;

    default:

    bad_reg:

        printf ("arm_sysctl_write: Bad register offset 0x%x\n", (int)offset);

        return;

    }

}