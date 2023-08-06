static void vapic_write(void *opaque, target_phys_addr_t addr, uint64_t data,

                        unsigned int size)

{

    CPUX86State *env = cpu_single_env;

    target_phys_addr_t rom_paddr;

    VAPICROMState *s = opaque;



    cpu_synchronize_state(env);



    

    switch (size) {

    case 2:

        if (s->state == VAPIC_INACTIVE) {

            rom_paddr = (env->segs[R_CS].base + env->eip) & ROM_BLOCK_MASK;

            s->rom_state_paddr = rom_paddr + data;



            s->state = VAPIC_STANDBY;

        }

        if (vapic_prepare(s) < 0) {

            s->state = VAPIC_INACTIVE;

            break;

        }

        break;

    case 1:

        if (kvm_enabled()) {

            

            pause_all_vcpus();

            patch_byte(env, env->eip - 2, 0x66);

            patch_byte(env, env->eip - 1, 0x90);

            resume_all_vcpus();

        }



        if (s->state == VAPIC_ACTIVE) {

            break;

        }

        if (update_rom_mapping(s, env, env->eip) < 0) {

            break;

        }

        if (find_real_tpr_addr(s, env) < 0) {

            break;

        }

        vapic_enable(s, env);

        break;

    default:

    case 4:

        if (!kvm_irqchip_in_kernel()) {

            apic_poll_irq(env->apic_state);

        }

        break;

    }

}