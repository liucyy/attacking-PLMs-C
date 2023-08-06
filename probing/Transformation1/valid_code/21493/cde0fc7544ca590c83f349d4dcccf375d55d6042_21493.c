static void handler_audit(Monitor *mon, const mon_cmd_t *cmd, int ret)

{

    if (ret && !monitor_has_error(mon)) {

        

        if (monitor_ctrl_mode(mon)) {

            qerror_report(QERR_UNDEFINED_ERROR);

        }

        MON_DEBUG("command '%s' returned failure but did not pass an error\n",

                  cmd->name);

    }



#ifdef CONFIG_DEBUG_MONITOR

    if (!ret && monitor_has_error(mon)) {

        

        MON_DEBUG("command '%s' returned success but passed an error\n",

                  cmd->name);

    }



    if (mon_print_count_get(mon) > 0 && strcmp(cmd->name, "info") != 0) {

        

        MON_DEBUG("command '%s' called print functions %d time(s)\n",

                  cmd->name, mon_print_count_get(mon));

    }

#endif

}