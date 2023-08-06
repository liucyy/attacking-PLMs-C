void kvm_arch_init_irq_routing(KVMState *s)

{

    if (!kvm_check_extension(s, KVM_CAP_IRQ_ROUTING)) {

        

        no_hpet = 1;

    }

    

    kvm_irqfds_allowed = true;

    kvm_msi_via_irqfd_allowed = true;

    kvm_gsi_routing_allowed = true;

}