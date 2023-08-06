void kvmppc_set_papr(CPUPPCState *env)

{

    struct kvm_enable_cap cap = {};

    struct kvm_one_reg reg = {};

    struct kvm_sregs sregs = {};

    int ret;

    uint64_t hior = env->spr[SPR_HIOR];



    cap.cap = KVM_CAP_PPC_PAPR;

    ret = kvm_vcpu_ioctl(env, KVM_ENABLE_CAP, &cap);



    if (ret) {

        goto fail;

    }



    

    reg.id = KVM_REG_PPC_HIOR;

    reg.addr = (uintptr_t)&hior;

    ret = kvm_vcpu_ioctl(env, KVM_SET_ONE_REG, &reg);

    if (ret) {

        fprintf(stderr, "Couldn't set HIOR. Maybe you're running an old \n"

                        "kernel with support for HV KVM but no PAPR PR \n"

                        "KVM in which case things will work. If they don't \n"

                        "please update your host kernel!\n");

    }



    

    ret = kvm_vcpu_ioctl(env, KVM_GET_SREGS, &sregs);

    if (ret) {

        goto fail;

    }



    sregs.u.s.sdr1 = env->spr[SPR_SDR1];



    ret = kvm_vcpu_ioctl(env, KVM_SET_SREGS, &sregs);

    if (ret) {

        goto fail;

    }



    return;



fail:

    cpu_abort(env, "This KVM version does not support PAPR\n");

}