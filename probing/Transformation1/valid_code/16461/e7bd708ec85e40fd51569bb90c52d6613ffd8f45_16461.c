void ide_atapi_cmd(IDEState *s)

{

    uint8_t *buf = s->io_buffer;

    const struct AtapiCmd *cmd = &atapi_cmd_table[s->io_buffer[0]];



#ifdef DEBUG_IDE_ATAPI

    {

        int i;

        printf("ATAPI limit=0x%x packet:", s->lcyl | (s->hcyl << 8));

        for(i = 0; i < ATAPI_PACKET_SIZE; i++) {

            printf(" %02x", buf[i]);

        }

        printf("\n");

    }

#endif



    

    if (s->sense_key == UNIT_ATTENTION && !(cmd->flags & ALLOW_UA)) {

        ide_atapi_cmd_check_status(s);

        return;

    }

    

    if (!(cmd->flags & ALLOW_UA) &&

        !s->tray_open && blk_is_inserted(s->blk) && s->cdrom_changed) {



        if (s->cdrom_changed == 1) {

            ide_atapi_cmd_error(s, NOT_READY, ASC_MEDIUM_NOT_PRESENT);

            s->cdrom_changed = 2;

        } else {

            ide_atapi_cmd_error(s, UNIT_ATTENTION, ASC_MEDIUM_MAY_HAVE_CHANGED);

            s->cdrom_changed = 0;

        }



        return;

    }



    

    if ((cmd->flags & CHECK_READY) &&

        (!media_present(s) || !blk_is_inserted(s->blk)))

    {

        ide_atapi_cmd_error(s, NOT_READY, ASC_MEDIUM_NOT_PRESENT);

        return;

    }



    

    if (cmd->handler && !(cmd->flags & NONDATA)) {

        

        if (!(atapi_byte_count_limit(s) || s->atapi_dma)) {

            

            ide_abort_command(s);

            return;

        }

    }



    

    if (cmd->handler) {

        cmd->handler(s, buf);

        return;

    }



    ide_atapi_cmd_error(s, ILLEGAL_REQUEST, ASC_ILLEGAL_OPCODE);

}