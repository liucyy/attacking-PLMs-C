static bool rtas_event_log_contains(uint32_t event_mask, bool exception)

{

    sPAPRMachineState *spapr = SPAPR_MACHINE(qdev_get_machine());

    sPAPREventLogEntry *entry = NULL;



    

    if ((event_mask & EVENT_MASK_EPOW) == 0) {

        return false;

    }



    QTAILQ_FOREACH(entry, &spapr->pending_events, next) {

        if (entry->exception != exception) {

            continue;

        }



        

        if (entry->log_type == RTAS_LOG_TYPE_EPOW ||

            entry->log_type == RTAS_LOG_TYPE_HOTPLUG) {

            return true;

        }

    }



    return false;

}