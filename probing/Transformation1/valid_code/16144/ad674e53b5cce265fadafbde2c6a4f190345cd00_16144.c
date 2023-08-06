static int conditional_wait(DBDMA_channel *ch)

{

    dbdma_cmd *current = &ch->current;

    uint16_t wait;

    uint16_t sel_mask, sel_value;

    uint32_t status;

    int cond;



    DBDMA_DPRINTF("conditional_wait\n");



    wait = le16_to_cpu(current->command) & WAIT_MASK;



    switch(wait) {

    case WAIT_NEVER:  

        return 0;

    case WAIT_ALWAYS: 

        return 1;

    }



    status = be32_to_cpu(ch->regs[DBDMA_STATUS]) & DEVSTAT;



    sel_mask = (be32_to_cpu(ch->regs[DBDMA_WAIT_SEL]) >> 16) & 0x0f;

    sel_value = be32_to_cpu(ch->regs[DBDMA_WAIT_SEL]) & 0x0f;



    cond = (status & sel_mask) == (sel_value & sel_mask);



    switch(wait) {

    case WAIT_IFSET:  

        if (cond)

            return 1;

        return 0;

    case WAIT_IFCLR:  

        if (!cond)

            return 1;

        return 0;

    }

    return 0;

}