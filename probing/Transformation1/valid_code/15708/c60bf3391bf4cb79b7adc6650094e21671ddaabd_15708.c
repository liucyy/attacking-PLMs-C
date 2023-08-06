static void monitor_find_completion(Monitor *mon,

                                    const char *cmdline)

{

    char *args[MAX_ARGS];

    int nb_args, len;



    

    if (parse_cmdline(cmdline, &nb_args, args) < 0) {

        return;

    }

#ifdef DEBUG_COMPLETION

    for (i = 0; i < nb_args; i++) {

        monitor_printf(mon, "arg%d = '%s'\n", i, args[i]);

    }

#endif



    

    len = strlen(cmdline);

    if (len > 0 && qemu_isspace(cmdline[len - 1])) {

        if (nb_args >= MAX_ARGS) {

            goto cleanup;

        }

        args[nb_args++] = g_strdup("");

    }



    

    monitor_find_completion_by_table(mon, mon->cmd_table, args, nb_args);



cleanup:

    free_cmdline_args(args, nb_args);

}