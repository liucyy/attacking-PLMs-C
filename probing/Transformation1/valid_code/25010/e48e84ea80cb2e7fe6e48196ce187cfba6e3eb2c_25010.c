static void scsi_cd_change_media_cb(void *opaque, bool load)

{

    SCSIDiskState *s = opaque;



    

    s->media_changed = load;

    s->tray_open = !load;

    s->qdev.unit_attention = SENSE_CODE(UNIT_ATTENTION_NO_MEDIUM);

    s->media_event = true;

    s->eject_request = false;

}