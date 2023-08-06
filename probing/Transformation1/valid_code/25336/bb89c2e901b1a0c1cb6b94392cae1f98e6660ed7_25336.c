static void monitor_call_handler(Monitor *mon, const mon_cmd_t *cmd,

                                 const QDict *params)

{

    QObject *data = NULL;



    cmd->mhandler.cmd_new(mon, params, &data);



    if (is_async_return(data)) {

        

        if (monitor_ctrl_mode(mon) && monitor_has_error(mon)) {

            monitor_protocol_emitter(mon, NULL);

        }

    } else if (monitor_ctrl_mode(mon)) {

        

        monitor_protocol_emitter(mon, data);

    } else {

        

         if (data)

            cmd->user_print(mon, data);

    }



    qobject_decref(data);

}