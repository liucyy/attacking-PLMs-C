static bool kvmppc_is_pr(KVMState *ks)

{

    

    return kvm_check_extension(ks, KVM_CAP_PPC_GET_PVINFO) != 0;

}