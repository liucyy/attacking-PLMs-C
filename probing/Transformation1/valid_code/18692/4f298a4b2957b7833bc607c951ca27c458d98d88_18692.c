static void clear_sel(IPMIBmcSim *ibs,

                      uint8_t *cmd, unsigned int cmd_len,

                      uint8_t *rsp, unsigned int *rsp_len,

                      unsigned int max_rsp_len)

{

    IPMI_CHECK_CMD_LEN(8);

    IPMI_CHECK_RESERVATION(2, ibs->sel.reservation);

    if (cmd[4] != 'C' || cmd[5] != 'L' || cmd[6] != 'R') {

        rsp[2] = IPMI_CC_INVALID_DATA_FIELD;

        return;

    }

    if (cmd[7] == 0xaa) {

        ibs->sel.next_free = 0;

        ibs->sel.overflow = 0;

        set_timestamp(ibs, ibs->sdr.last_clear);

        IPMI_ADD_RSP_DATA(1); 

        sel_inc_reservation(&ibs->sel);

    } else if (cmd[7] == 0) {

        IPMI_ADD_RSP_DATA(1); 

    } else {

        rsp[2] = IPMI_CC_INVALID_DATA_FIELD;

        return;

    }

}