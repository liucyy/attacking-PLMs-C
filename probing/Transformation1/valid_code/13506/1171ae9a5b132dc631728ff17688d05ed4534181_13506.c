void parse_numa_opts(MachineState *ms)

{

    int i;

    const CPUArchIdList *possible_cpus;

    MachineClass *mc = MACHINE_GET_CLASS(ms);



    for (i = 0; i < MAX_NODES; i++) {

        numa_info[i].node_cpu = bitmap_new(max_cpus);

    }



    if (qemu_opts_foreach(qemu_find_opts("numa"), parse_numa, ms, NULL)) {

        exit(1);

    }



    assert(max_numa_nodeid <= MAX_NODES);



    

    for (i = max_numa_nodeid - 1; i >= 0; i--) {

        

        if (!numa_info[i].present) {

            error_report("numa: Node ID missing: %d", i);

            exit(1);

        }

    }



    

    assert(nb_numa_nodes == max_numa_nodeid);



    if (nb_numa_nodes > 0) {

        uint64_t numa_total;



        if (nb_numa_nodes > MAX_NODES) {

            nb_numa_nodes = MAX_NODES;

        }



        

        for (i = 0; i < nb_numa_nodes; i++) {

            if (numa_info[i].node_mem != 0) {

                break;

            }

        }

        if (i == nb_numa_nodes) {

            assert(mc->numa_auto_assign_ram);

            mc->numa_auto_assign_ram(mc, numa_info, nb_numa_nodes, ram_size);

        }



        numa_total = 0;

        for (i = 0; i < nb_numa_nodes; i++) {

            numa_total += numa_info[i].node_mem;

        }

        if (numa_total != ram_size) {

            error_report("total memory for NUMA nodes (0x%" PRIx64 ")"

                         " should equal RAM size (0x" RAM_ADDR_FMT ")",

                         numa_total, ram_size);

            exit(1);

        }



        for (i = 0; i < nb_numa_nodes; i++) {

            QLIST_INIT(&numa_info[i].addr);

        }



        numa_set_mem_ranges();



        

        if (!mc->cpu_index_to_instance_props || !mc->possible_cpu_arch_ids) {

            error_report("default CPUs to NUMA node mapping isn't supported");

            exit(1);

        }



        possible_cpus = mc->possible_cpu_arch_ids(ms);

        for (i = 0; i < possible_cpus->len; i++) {

            if (possible_cpus->cpus[i].props.has_node_id) {

                break;

            }

        }



        

        if (i == possible_cpus->len) {

            for (i = 0; i < max_cpus; i++) {

                CpuInstanceProperties props;

                

                props = mc->cpu_index_to_instance_props(ms, i);

                props.has_node_id = true;



                set_bit(i, numa_info[props.node_id].node_cpu);

                machine_set_cpu_numa_node(ms, &props, &error_fatal);

            }

        }



        validate_numa_cpus();



        

        if (have_numa_distance) {

            

            validate_numa_distance();



            

            complete_init_numa_distance();

        }

    } else {

        numa_set_mem_node_id(0, ram_size, 0);

    }

}