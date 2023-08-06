static sd_rsp_type_t sd_app_command(SDState *sd,

                                    SDRequest req)

{

    DPRINTF("ACMD%d 0x%08x\n", req.cmd, req.arg);

    sd->card_status |= APP_CMD;

    switch (req.cmd) {

    case 6:	

        switch (sd->state) {

        case sd_transfer_state:

            sd->sd_status[0] &= 0x3f;

            sd->sd_status[0] |= (req.arg & 0x03) << 6;

            return sd_r1;



        default:

            break;

        }

        break;



    case 13:	

        switch (sd->state) {

        case sd_transfer_state:

            sd->state = sd_sendingdata_state;

            sd->data_start = 0;

            sd->data_offset = 0;

            return sd_r1;



        default:

            break;

        }

        break;



    case 22:	

        switch (sd->state) {

        case sd_transfer_state:

            *(uint32_t *) sd->data = sd->blk_written;



            sd->state = sd_sendingdata_state;

            sd->data_start = 0;

            sd->data_offset = 0;

            return sd_r1;



        default:

            break;

        }

        break;



    case 23:	

        switch (sd->state) {

        case sd_transfer_state:

            return sd_r1;



        default:

            break;

        }

        break;



    case 41:	

        if (sd->spi) {

            

            sd->state = sd_transfer_state;

            return sd_r1;

        }

        switch (sd->state) {

        case sd_idle_state:

            

            if (req.arg & ACMD41_ENQUIRY_MASK) {

                sd->state = sd_ready_state;

            }



            return sd_r3;



        default:

            break;

        }

        break;



    case 42:	

        switch (sd->state) {

        case sd_transfer_state:

            

            return sd_r1;



        default:

            break;

        }

        break;



    case 51:	

        switch (sd->state) {

        case sd_transfer_state:

            sd->state = sd_sendingdata_state;

            sd->data_start = 0;

            sd->data_offset = 0;

            return sd_r1;



        default:

            break;

        }

        break;



    default:

        

        return sd_normal_command(sd, req);

    }



    fprintf(stderr, "SD: ACMD%i in a wrong state\n", req.cmd);

    return sd_illegal;

}