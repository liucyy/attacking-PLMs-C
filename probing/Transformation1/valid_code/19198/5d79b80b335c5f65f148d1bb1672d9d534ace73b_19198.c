static void pflash_write(pflash_t *pfl, hwaddr offset,

                         uint32_t value, int width, int be)

{

    uint8_t *p;

    uint8_t cmd;



    cmd = value;



    DPRINTF("%s: writing offset " TARGET_FMT_plx " value %08x width %d wcycle 0x%x\n",

            __func__, offset, value, width, pfl->wcycle);



    if (!pfl->wcycle) {

        

        memory_region_rom_device_set_readable(&pfl->mem, false);

    }



    switch (pfl->wcycle) {

    case 0:

        

        switch (cmd) {

        case 0x00: 

            goto reset_flash;

        case 0x10: 

        case 0x40: 

            DPRINTF("%s: Single Byte Program\n", __func__);

            break;

        case 0x20: 

            p = pfl->storage;

            offset &= ~(pfl->sector_len - 1);



            DPRINTF("%s: block erase at " TARGET_FMT_plx " bytes %x\n",

                    __func__, offset, (unsigned)pfl->sector_len);



            if (!pfl->ro) {

                memset(p + offset, 0xff, pfl->sector_len);

                pflash_update(pfl, offset, pfl->sector_len);

            } else {

                pfl->status |= 0x20; 

            }

            pfl->status |= 0x80; 

            break;

        case 0x50: 

            DPRINTF("%s: Clear status bits\n", __func__);

            pfl->status = 0x0;

            goto reset_flash;

        case 0x60: 

            DPRINTF("%s: Block unlock\n", __func__);

            break;

        case 0x70: 

            DPRINTF("%s: Read status register\n", __func__);

            pfl->cmd = cmd;

            return;

        case 0x90: 

            DPRINTF("%s: Read Device information\n", __func__);

            pfl->cmd = cmd;

            return;

        case 0x98: 

            DPRINTF("%s: CFI query\n", __func__);

            break;

        case 0xe8: 

            DPRINTF("%s: Write to buffer\n", __func__);

            pfl->status |= 0x80; 

            break;

        case 0xf0: 

            DPRINTF("%s: Probe for AMD flash\n", __func__);

            goto reset_flash;

        case 0xff: 

            DPRINTF("%s: Read array mode\n", __func__);

            goto reset_flash;

        default:

            goto error_flash;

        }

        pfl->wcycle++;

        pfl->cmd = cmd;

        break;

    case 1:

        switch (pfl->cmd) {

        case 0x10: 

        case 0x40: 

            DPRINTF("%s: Single Byte Program\n", __func__);

            if (!pfl->ro) {

                pflash_data_write(pfl, offset, value, width, be);

                pflash_update(pfl, offset, width);

            } else {

                pfl->status |= 0x10; 

            }

            pfl->status |= 0x80; 

            pfl->wcycle = 0;

        break;

        case 0x20: 

        case 0x28:

            if (cmd == 0xd0) { 

                pfl->wcycle = 0;

                pfl->status |= 0x80;

            } else if (cmd == 0xff) { 

                goto reset_flash;

            } else

                goto error_flash;



            break;

        case 0xe8:

            DPRINTF("%s: block write of %x bytes\n", __func__, value);

            pfl->counter = value;

            pfl->wcycle++;

            break;

        case 0x60:

            if (cmd == 0xd0) {

                pfl->wcycle = 0;

                pfl->status |= 0x80;

            } else if (cmd == 0x01) {

                pfl->wcycle = 0;

                pfl->status |= 0x80;

            } else if (cmd == 0xff) {

                goto reset_flash;

            } else {

                DPRINTF("%s: Unknown (un)locking command\n", __func__);

                goto reset_flash;

            }

            break;

        case 0x98:

            if (cmd == 0xff) {

                goto reset_flash;

            } else {

                DPRINTF("%s: leaving query mode\n", __func__);

            }

            break;

        default:

            goto error_flash;

        }

        break;

    case 2:

        switch (pfl->cmd) {

        case 0xe8: 

            if (!pfl->ro) {

                pflash_data_write(pfl, offset, value, width, be);

            } else {

                pfl->status |= 0x10; 

            }



            pfl->status |= 0x80;



            if (!pfl->counter) {

                hwaddr mask = pfl->writeblock_size - 1;

                mask = ~mask;



                DPRINTF("%s: block write finished\n", __func__);

                pfl->wcycle++;

                if (!pfl->ro) {

                    

                    pflash_update(pfl, offset & mask, pfl->writeblock_size);

                } else {

                    pfl->status |= 0x10; 

                }

            }



            pfl->counter--;

            break;

        default:

            goto error_flash;

        }

        break;

    case 3: 

        switch (pfl->cmd) {

        case 0xe8: 

            if (cmd == 0xd0) {

                pfl->wcycle = 0;

                pfl->status |= 0x80;

            } else {

                DPRINTF("%s: unknown command for \"write block\"\n", __func__);

                PFLASH_BUG("Write block confirm");

                goto reset_flash;

            }

            break;

        default:

            goto error_flash;

        }

        break;

    default:

        

        DPRINTF("%s: invalid write state\n",  __func__);

        goto reset_flash;

    }

    return;



 error_flash:

    qemu_log_mask(LOG_UNIMP, "%s: Unimplemented flash cmd sequence "

                  "(offset " TARGET_FMT_plx ", wcycle 0x%x cmd 0x%x value 0x%x)"

                  "\n", __func__, offset, pfl->wcycle, pfl->cmd, value);



 reset_flash:

    memory_region_rom_device_set_readable(&pfl->mem, true);



    pfl->bypass = 0;

    pfl->wcycle = 0;

    pfl->cmd = 0;

}