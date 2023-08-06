static int handle_cmd(AHCIState *s, int port, int slot)

{

    IDEState *ide_state;

    uint32_t opts;

    uint64_t tbl_addr;

    AHCICmdHdr *cmd;

    uint8_t *cmd_fis;

    dma_addr_t cmd_len;



    if (s->dev[port].port.ifs[0].status & (BUSY_STAT|DRQ_STAT)) {

        

        DPRINTF(port, "engine busy\n");

        return -1;

    }



    cmd = &((AHCICmdHdr *)s->dev[port].lst)[slot];



    if (!s->dev[port].lst) {

        DPRINTF(port, "error: lst not given but cmd handled");

        return -1;

    }



    

    s->dev[port].cur_cmd = cmd;



    opts = le32_to_cpu(cmd->opts);

    tbl_addr = le64_to_cpu(cmd->tbl_addr);



    cmd_len = 0x80;

    cmd_fis = dma_memory_map(s->as, tbl_addr, &cmd_len,

                             DMA_DIRECTION_FROM_DEVICE);



    if (!cmd_fis) {

        DPRINTF(port, "error: guest passed us an invalid cmd fis\n");

        return -1;

    }



    

    ide_state = &s->dev[port].port.ifs[0];



    if (!ide_state->blk) {

        DPRINTF(port, "error: guest accessed unused port");

        goto out;

    }



    debug_print_fis(cmd_fis, 0x90);

    



    switch (cmd_fis[0]) {

        case SATA_FIS_TYPE_REGISTER_H2D:

            break;

        default:

            DPRINTF(port, "unknown command cmd_fis[0]=%02x cmd_fis[1]=%02x "

                          "cmd_fis[2]=%02x\n", cmd_fis[0], cmd_fis[1],

                          cmd_fis[2]);

            goto out;

            break;

    }



    switch (cmd_fis[1]) {

        case SATA_FIS_REG_H2D_UPDATE_COMMAND_REGISTER:

            break;

        case 0:

            break;

        default:

            DPRINTF(port, "unknown command cmd_fis[0]=%02x cmd_fis[1]=%02x "

                          "cmd_fis[2]=%02x\n", cmd_fis[0], cmd_fis[1],

                          cmd_fis[2]);

            goto out;

            break;

    }



    if (!(cmd_fis[1] & SATA_FIS_REG_H2D_UPDATE_COMMAND_REGISTER)) {

        switch (s->dev[port].port_state) {

        case STATE_RUN:

            if (cmd_fis[15] & ATA_SRST) {

                s->dev[port].port_state = STATE_RESET;

            }

            break;

        case STATE_RESET:

            if (!(cmd_fis[15] & ATA_SRST)) {

                ahci_reset_port(s, port);

            }

            break;

        }

    }



    else if (cmd_fis[1] & SATA_FIS_REG_H2D_UPDATE_COMMAND_REGISTER) {



        

        if (is_ncq(cmd_fis[2])) {

            process_ncq_command(s, port, cmd_fis, slot);

            goto out;

        }



        

        ide_state->feature = cmd_fis[3];

        ide_state->sector = cmd_fis[4];     

        ide_state->lcyl = cmd_fis[5];       

        ide_state->hcyl = cmd_fis[6];       

        ide_state->select = cmd_fis[7];     

        ide_state->hob_sector = cmd_fis[8]; 

        ide_state->hob_lcyl = cmd_fis[9];   

        ide_state->hob_hcyl = cmd_fis[10];  

        ide_state->hob_feature = cmd_fis[11];

        ide_state->nsector = (int64_t)((cmd_fis[13] << 8) | cmd_fis[12]);

        

        



        

        if (opts & AHCI_CMD_ATAPI) {

            memcpy(ide_state->io_buffer, &cmd_fis[AHCI_COMMAND_TABLE_ACMD], 0x10);

            debug_print_fis(ide_state->io_buffer, 0x10);

            s->dev[port].done_atapi_packet = false;

            

        }



        ide_state->error = 0;



        

        cmd->status = 0;



        

        ide_exec_cmd(&s->dev[port].port, cmd_fis[2]);

    }



out:

    dma_memory_unmap(s->as, cmd_fis, cmd_len, DMA_DIRECTION_FROM_DEVICE,

                     cmd_len);



    if (s->dev[port].port.ifs[0].status & (BUSY_STAT|DRQ_STAT)) {

        

        s->dev[port].busy_slot = slot;

        return -1;

    }



    

    return 0;

}