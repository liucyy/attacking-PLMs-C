static void kvm_arm_gic_put(GICState *s)

{

    uint32_t reg;

    int i;

    int cpu;

    int num_cpu;

    int num_irq;



    if (!kvm_arm_gic_can_save_restore(s)) {

            DPRINTF("Cannot put kernel gic state, no kernel interface");

            return;

    }



    



    



    

    reg = s->enabled;

    kvm_gicd_access(s, 0x0, 0, &reg, true);



    

    kvm_gicd_access(s, 0x4, 0, &reg, false);

    num_irq = ((reg & 0x1f) + 1) * 32;

    num_cpu = ((reg & 0xe0) >> 5) + 1;



    if (num_irq < s->num_irq) {

            fprintf(stderr, "Restoring %u IRQs, but kernel supports max %d\n",

                    s->num_irq, num_irq);

            abort();

    } else if (num_cpu != s->num_cpu) {

            fprintf(stderr, "Restoring %u CPU interfaces, kernel only has %d\n",

                    s->num_cpu, num_cpu);

            

            abort();

    }



    



    

    kvm_dist_put(s, 0x180, 1, s->num_irq, translate_clear);

    kvm_dist_put(s, 0x100, 1, s->num_irq, translate_enabled);



    

    kvm_dist_put(s, 0x80, 1, s->num_irq, translate_group);



    

    kvm_dist_put(s, 0x800, 8, s->num_irq, translate_targets);



    

    kvm_dist_put(s, 0xc00, 2, s->num_irq, translate_trigger);



    

    kvm_dist_put(s, 0x280, 1, s->num_irq, translate_clear);

    kvm_dist_put(s, 0x200, 1, s->num_irq, translate_pending);



    

    kvm_dist_put(s, 0x380, 1, s->num_irq, translate_clear);

    kvm_dist_put(s, 0x300, 1, s->num_irq, translate_active);





    

    kvm_dist_put(s, 0x400, 8, s->num_irq, translate_priority);



    

    kvm_dist_put(s, 0xf10, 8, GIC_NR_SGIS, translate_clear);

    kvm_dist_put(s, 0xf20, 8, GIC_NR_SGIS, translate_sgisource);





    



    for (cpu = 0; cpu < s->num_cpu; cpu++) {

        

        reg = s->cpu_enabled[cpu];

        kvm_gicc_access(s, 0x00, cpu, &reg, true);



        

        reg = (s->priority_mask[cpu] & 0xff);

        kvm_gicc_access(s, 0x04, cpu, &reg, true);



        

        reg = (s->bpr[cpu] & 0x7);

        kvm_gicc_access(s, 0x08, cpu, &reg, true);



        

        reg = (s->abpr[cpu] & 0x7);

        kvm_gicc_access(s, 0x1c, cpu, &reg, true);



        

        for (i = 0; i < 4; i++) {

            reg = s->apr[i][cpu];

            kvm_gicc_access(s, 0xd0 + i * 4, cpu, &reg, true);

        }

    }

}