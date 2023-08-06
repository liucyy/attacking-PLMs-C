static int cpu_post_load(void *opaque, int version_id)

{

    ARMCPU *cpu = opaque;

    int i, v;



    



    for (i = 0, v = 0; i < cpu->cpreg_array_len

             && v < cpu->cpreg_vmstate_array_len; i++) {

        if (cpu->cpreg_vmstate_indexes[v] > cpu->cpreg_indexes[i]) {

            

            continue;

        }

        if (cpu->cpreg_vmstate_indexes[v] < cpu->cpreg_indexes[i]) {

            

            return -1;

        }

        

        cpu->cpreg_values[i] = cpu->cpreg_vmstate_values[v];

        v++;

    }



    if (kvm_enabled()) {

        if (!write_list_to_kvmstate(cpu)) {

            return -1;

        }

        

        write_list_to_cpustate(cpu);

    } else {

        if (!write_list_to_cpustate(cpu)) {

            return -1;

        }

    }



    hw_breakpoint_update_all(cpu);

    hw_watchpoint_update_all(cpu);



    return 0;

}