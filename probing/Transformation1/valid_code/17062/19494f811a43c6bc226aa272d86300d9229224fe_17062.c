qemu_irq get_cps_irq(MIPSCPSState *s, int pin_number)

{

    MIPSCPU *cpu = MIPS_CPU(first_cpu);

    CPUMIPSState *env = &cpu->env;



    assert(pin_number < s->num_irq);



    

    return env->irq[pin_number];

}