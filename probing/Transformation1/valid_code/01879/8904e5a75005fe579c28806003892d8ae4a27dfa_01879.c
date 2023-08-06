static void spapr_set_vsmt_mode(sPAPRMachineState *spapr, Error **errp)

{

    Error *local_err = NULL;

    bool vsmt_user = !!spapr->vsmt;

    int kvm_smt = kvmppc_smt_threads();

    int ret;



    if (!kvm_enabled() && (smp_threads > 1)) {

        error_setg(&local_err, "TCG cannot support more than 1 thread/core "

                     "on a pseries machine");

        goto out;

    }

    if (!is_power_of_2(smp_threads)) {

        error_setg(&local_err, "Cannot support %d threads/core on a pseries "

                     "machine because it must be a power of 2", smp_threads);

        goto out;

    }



    

    if (vsmt_user) {

        if (spapr->vsmt < smp_threads) {

            error_setg(&local_err, "Cannot support VSMT mode %d"

                         " because it must be >= threads/core (%d)",

                         spapr->vsmt, smp_threads);

            goto out;

        }

        

    } else {

        

        spapr->vsmt = MAX(kvm_smt, smp_threads);

    }



    

    if (kvm_enabled() && (spapr->vsmt != kvm_smt)) {

        ret = kvmppc_set_smt_threads(spapr->vsmt);

        if (ret) {

            

            error_setg(&local_err,

                       "Failed to set KVM's VSMT mode to %d (errno %d)",

                       spapr->vsmt, ret);

            

            if ((kvm_smt >= smp_threads) && ((spapr->vsmt % kvm_smt) == 0)) {

                warn_report_err(local_err);

                local_err = NULL;

                goto out;

            } else {

                if (!vsmt_user) {

                    error_append_hint(&local_err,

                                      "On PPC, a VM with %d threads/core"

                                      " on a host with %d threads/core"

                                      " requires the use of VSMT mode %d.\n",

                                      smp_threads, kvm_smt, spapr->vsmt);

                }

                kvmppc_hint_smt_possible(&local_err);

                goto out;

            }

        }

    }

    

out:

    error_propagate(errp, local_err);

}