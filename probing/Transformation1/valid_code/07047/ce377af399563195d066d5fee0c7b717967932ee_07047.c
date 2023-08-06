void kvm_arch_pre_run(CPUState *env, struct kvm_run *run)

{

    

    if (env->interrupt_request & CPU_INTERRUPT_NMI) {

        env->interrupt_request &= ~CPU_INTERRUPT_NMI;

        DPRINTF("injected NMI\n");

        kvm_vcpu_ioctl(env, KVM_NMI);

    }



    if (!kvm_irqchip_in_kernel()) {

        

        if (env->interrupt_request & CPU_INTERRUPT_INIT) {

            env->exit_request = 1;

        }



        

        if (run->ready_for_interrupt_injection &&

            (env->interrupt_request & CPU_INTERRUPT_HARD) &&

            (env->eflags & IF_MASK)) {

            int irq;



            env->interrupt_request &= ~CPU_INTERRUPT_HARD;

            irq = cpu_get_pic_interrupt(env);

            if (irq >= 0) {

                struct kvm_interrupt intr;



                intr.irq = irq;

                

                DPRINTF("injected interrupt %d\n", irq);

                kvm_vcpu_ioctl(env, KVM_INTERRUPT, &intr);

            }

        }



        

        if ((env->interrupt_request & CPU_INTERRUPT_HARD)) {

            run->request_interrupt_window = 1;

        } else {

            run->request_interrupt_window = 0;

        }



        DPRINTF("setting tpr\n");

        run->cr8 = cpu_get_apic_tpr(env->apic_state);

    }

}