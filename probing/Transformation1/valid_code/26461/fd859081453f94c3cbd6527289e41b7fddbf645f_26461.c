static void tpm_tis_abort(TPMState *s, uint8_t locty)

{

    TPMTISEmuState *tis = &s->s.tis;



    tis->loc[locty].r_offset = 0;

    tis->loc[locty].w_offset = 0;



    DPRINTF("tpm_tis: tis_abort: new active locality is %d\n", tis->next_locty);



    

    if (tis->aborting_locty == tis->next_locty) {

        tis->loc[tis->aborting_locty].state = TPM_TIS_STATE_READY;

        tis->loc[tis->aborting_locty].sts = TPM_TIS_STS_COMMAND_READY;

        tpm_tis_raise_irq(s, tis->aborting_locty, TPM_TIS_INT_COMMAND_READY);

    }



    

    tpm_tis_new_active_locality(s, tis->next_locty);



    tis->next_locty = TPM_TIS_NO_LOCALITY;

    

    tis->aborting_locty = TPM_TIS_NO_LOCALITY;

}