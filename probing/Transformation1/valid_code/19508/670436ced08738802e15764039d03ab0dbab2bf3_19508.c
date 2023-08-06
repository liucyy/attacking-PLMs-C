static int kvm_max_vcpus(KVMState *s)

{

    int ret;



    

    ret = kvm_check_extension(s, KVM_CAP_MAX_VCPUS);

    if (ret) {

        return ret;

    }

    ret = kvm_check_extension(s, KVM_CAP_NR_VCPUS);

    if (ret) {

        return ret;

    }



    return 4;

}