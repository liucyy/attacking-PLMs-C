void kvmppc_check_papr_resize_hpt(Error **errp)

{

    if (!kvm_enabled()) {

        return;

    }



    



    error_setg(errp,

               "Hash page table resizing not available with this KVM version");

}