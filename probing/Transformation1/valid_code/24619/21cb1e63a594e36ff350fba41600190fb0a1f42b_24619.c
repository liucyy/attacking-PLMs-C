static void tpm_passthrough_cancel_cmd(TPMBackend *tb)

{

    TPMPassthruState *tpm_pt = TPM_PASSTHROUGH(tb);

    int n;



    

    if (tpm_pt->tpm_executing) {

        if (tpm_pt->cancel_fd >= 0) {

            n = write(tpm_pt->cancel_fd, "-", 1);

            if (n != 1) {

                error_report("Canceling TPM command failed: %s",

                             strerror(errno));

            } else {

                tpm_pt->tpm_op_canceled = true;

            }

        } else {

            error_report("Cannot cancel TPM command due to missing "

                         "TPM sysfs cancel entry");

        }

    }

}