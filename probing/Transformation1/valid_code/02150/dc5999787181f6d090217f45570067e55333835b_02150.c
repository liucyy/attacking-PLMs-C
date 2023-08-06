monitor_qapi_event_queue(QAPIEvent event, QDict *qdict, Error **errp)

{

    MonitorQAPIEventConf *evconf;

    MonitorQAPIEventState *evstate;



    assert(event < QAPI_EVENT__MAX);

    evconf = &monitor_qapi_event_conf[event];

    trace_monitor_protocol_event_queue(event, qdict, evconf->rate);



    qemu_mutex_lock(&monitor_lock);



    if (!evconf->rate) {

        

        monitor_qapi_event_emit(event, qdict);

    } else {

        QDict *data = qobject_to_qdict(qdict_get(qdict, "data"));

        MonitorQAPIEventState key = { .event = event, .data = data };



        evstate = g_hash_table_lookup(monitor_qapi_event_state, &key);

        assert(!evstate || timer_pending(evstate->timer));



        if (evstate) {

            

            QDECREF(evstate->qdict);

            evstate->qdict = qdict;

            QINCREF(evstate->qdict);

        } else {

            

            int64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);



            monitor_qapi_event_emit(event, qdict);



            evstate = g_new(MonitorQAPIEventState, 1);

            evstate->event = event;

            evstate->data = data;

            QINCREF(evstate->data);

            evstate->qdict = NULL;

            evstate->timer = timer_new_ns(QEMU_CLOCK_REALTIME,

                                          monitor_qapi_event_handler,

                                          evstate);

            g_hash_table_add(monitor_qapi_event_state, evstate);

            timer_mod_ns(evstate->timer, now + evconf->rate);

        }

    }



    qemu_mutex_unlock(&monitor_lock);

}