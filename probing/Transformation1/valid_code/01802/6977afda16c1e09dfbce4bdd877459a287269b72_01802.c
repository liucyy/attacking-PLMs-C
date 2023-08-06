int kvmppc_reset_htab(int shift_hint)

{

    uint32_t shift = shift_hint;



    if (!kvm_enabled()) {

        

        return 0;

    }

    if (kvm_check_extension(kvm_state, KVM_CAP_PPC_ALLOC_HTAB)) {

        int ret;

        ret = kvm_vm_ioctl(kvm_state, KVM_PPC_ALLOCATE_HTAB, &shift);

        if (ret == -ENOTTY) {

            

            return 0;

        } else if (ret < 0) {

            return ret;

        }

        return shift;

    }



    

    if (kvmppc_is_pr(kvm_state)) {

        

        return 0;

    } else {

        

        return 24;

    }

}