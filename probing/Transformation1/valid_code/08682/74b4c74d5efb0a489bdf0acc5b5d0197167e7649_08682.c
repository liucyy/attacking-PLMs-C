int kvm_arch_init(MachineState *ms, KVMState *s)

{

    MachineClass *mc = MACHINE_GET_CLASS(ms);



    mc->default_cpu_type = S390_CPU_TYPE_NAME("host");

    cap_sync_regs = kvm_check_extension(s, KVM_CAP_SYNC_REGS);

    cap_async_pf = kvm_check_extension(s, KVM_CAP_ASYNC_PF);

    cap_mem_op = kvm_check_extension(s, KVM_CAP_S390_MEM_OP);

    cap_s390_irq = kvm_check_extension(s, KVM_CAP_S390_INJECT_IRQ);



    if (!kvm_check_extension(s, KVM_CAP_S390_GMAP)

        || !kvm_check_extension(s, KVM_CAP_S390_COW)) {

        phys_mem_set_alloc(legacy_s390_alloc);

    }



    kvm_vm_enable_cap(s, KVM_CAP_S390_USER_SIGP, 0);

    kvm_vm_enable_cap(s, KVM_CAP_S390_VECTOR_REGISTERS, 0);

    kvm_vm_enable_cap(s, KVM_CAP_S390_USER_STSI, 0);

    if (ri_allowed()) {

        if (kvm_vm_enable_cap(s, KVM_CAP_S390_RI, 0) == 0) {

            cap_ri = 1;

        }

    }

    if (gs_allowed()) {

        if (kvm_vm_enable_cap(s, KVM_CAP_S390_GS, 0) == 0) {

            cap_gs = 1;

        }

    }



    

    



    qemu_mutex_init(&qemu_sigp_mutex);



    return 0;

}