static int gdb_handle_vcont(GDBState *s, const char *p)

{

    int res, idx, signal = 0;

    char cur_action;

    char *newstates;

    unsigned long tmp;

    CPUState *cpu;

#ifdef CONFIG_USER_ONLY

    int max_cpus = 1; 



    CPU_FOREACH(cpu) {

        max_cpus = max_cpus <= cpu->cpu_index ? cpu->cpu_index + 1 : max_cpus;

    }

#endif

    

    newstates = g_new0(char, max_cpus);



    

    CPU_FOREACH(cpu) {

        newstates[cpu->cpu_index] = 1;

    }



    

    res = 0;

    while (*p) {

        if (*p++ != ';') {

            res = -ENOTSUP;

            goto out;

        }



        cur_action = *p++;

        if (cur_action == 'C' || cur_action == 'S') {

            cur_action = tolower(cur_action);

            res = qemu_strtoul(p + 1, &p, 16, &tmp);

            if (res) {

                goto out;

            }

            signal = gdb_signal_to_target(tmp);

        } else if (cur_action != 'c' && cur_action != 's') {

            

            res = -ENOTSUP;

            goto out;

        }

        

        if ((p[0] == ':' && p[1] == '-' && p[2] == '1') || (p[0] != ':')) {

            if (*p == ':') {

                p += 3;

            }

            for (idx = 0; idx < max_cpus; idx++) {

                if (newstates[idx] == 1) {

                    newstates[idx] = cur_action;

                }

            }

        } else if (*p == ':') {

            p++;

            res = qemu_strtoul(p, &p, 16, &tmp);

            if (res) {

                goto out;

            }



            

            cpu = tmp ? find_cpu(tmp) : first_cpu;



            

            if (!cpu) {

                res = -EINVAL;

                goto out;

            }



            

            if (newstates[cpu->cpu_index] == 1) {

                newstates[cpu->cpu_index] = cur_action;

            }

        }

    }

    s->signal = signal;

    gdb_continue_partial(s, newstates);



out:

    g_free(newstates);



    return res;

}