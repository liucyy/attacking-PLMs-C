static void arm_cpu_initfn(Object *obj)

{

    CPUState *cs = CPU(obj);

    ARMCPU *cpu = ARM_CPU(obj);

    static bool inited;

    uint32_t Aff1, Aff0;



    cs->env_ptr = &cpu->env;

    cpu_exec_init(cs, &error_abort);

    cpu->cp_regs = g_hash_table_new_full(g_int_hash, g_int_equal,

                                         g_free, g_free);



    

    Aff1 = cs->cpu_index / ARM_CPUS_PER_CLUSTER;

    Aff0 = cs->cpu_index % ARM_CPUS_PER_CLUSTER;

    cpu->mp_affinity = (Aff1 << ARM_AFF1_SHIFT) | Aff0;



#ifndef CONFIG_USER_ONLY

    

    if (kvm_enabled()) {

        

        qdev_init_gpio_in(DEVICE(cpu), arm_cpu_kvm_set_irq, 4);

    } else {

        qdev_init_gpio_in(DEVICE(cpu), arm_cpu_set_irq, 4);

    }



    cpu->gt_timer[GTIMER_PHYS] = timer_new(QEMU_CLOCK_VIRTUAL, GTIMER_SCALE,

                                                arm_gt_ptimer_cb, cpu);

    cpu->gt_timer[GTIMER_VIRT] = timer_new(QEMU_CLOCK_VIRTUAL, GTIMER_SCALE,

                                                arm_gt_vtimer_cb, cpu);

    cpu->gt_timer[GTIMER_HYP] = timer_new(QEMU_CLOCK_VIRTUAL, GTIMER_SCALE,

                                                arm_gt_htimer_cb, cpu);

    cpu->gt_timer[GTIMER_SEC] = timer_new(QEMU_CLOCK_VIRTUAL, GTIMER_SCALE,

                                                arm_gt_stimer_cb, cpu);

    qdev_init_gpio_out(DEVICE(cpu), cpu->gt_timer_outputs,

                       ARRAY_SIZE(cpu->gt_timer_outputs));

#endif



    

    cpu->dtb_compatible = "qemu,unknown";

    cpu->psci_version = 1; 

    cpu->kvm_target = QEMU_KVM_ARM_TARGET_NONE;



    if (tcg_enabled()) {

        cpu->psci_version = 2; 

        if (!inited) {

            inited = true;

            arm_translate_init();

        }

    }

}