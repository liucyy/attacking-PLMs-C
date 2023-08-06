void kvm_arm_reset_vcpu(ARMCPU *cpu)

{

    

    kvm_arm_vcpu_init(CPU(cpu));

    write_kvmstate_to_list(cpu);

}