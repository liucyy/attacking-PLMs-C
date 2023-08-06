static uint32_t arm_sysctl_read(void *opaque, target_phys_addr_t offset)

{

    arm_sysctl_state *s = (arm_sysctl_state *)opaque;



    switch (offset) {

    case 0x00: 

        return s->sys_id;

    case 0x04: 

        

        return 0;

    case 0x08: 

        return s->leds;

    case 0x20: 

        return s->lockval;

    case 0x0c: 

    case 0x10: 

    case 0x14: 

    case 0x18: 

    case 0x1c: 

    case 0x24: 

        

        return 0;

    case 0x28: 

        return s->cfgdata1;

    case 0x2c: 

        return s->cfgdata2;

    case 0x30: 

        return s->flags;

    case 0x38: 

        return s->nvflags;

    case 0x40: 

        return s->resetlevel;

    case 0x44: 

        return 1;

    case 0x48: 

        return 0;

    case 0x4c: 

        return 0;

    case 0x50: 

        return 0x1000;

    case 0x54: 

        return 0;

    case 0x58: 

        return 0;

    case 0x5c: 

        return muldiv64(qemu_get_clock(vm_clock), 24000000, get_ticks_per_sec());

    case 0x60: 

        return 0;

    case 0x84: 

        

        return 0x02000000;

    case 0x88: 

        return 0xff000000;

    case 0x64: 

    case 0x68: 

    case 0x6c: 

    case 0x70: 

    case 0x74: 

    case 0x80: 

    case 0x8c: 

    case 0x90: 

    case 0x94: 

    case 0x98: 

    case 0x9c: 

    case 0xc0: 

    case 0xc4: 

    case 0xc8: 

    case 0xcc: 

    case 0xd0: 

        return 0;

    default:

        printf ("arm_sysctl_read: Bad register offset 0x%x\n", (int)offset);

        return 0;

    }

}