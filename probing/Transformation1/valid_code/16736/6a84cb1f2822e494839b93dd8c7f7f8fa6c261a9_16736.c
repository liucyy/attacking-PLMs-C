static int usb_msd_initfn(USBDevice *dev)

{

    MSDState *s = DO_UPCAST(MSDState, dev, dev);

    BlockDriverState *bs = s->conf.bs;

    DriveInfo *dinfo;



    if (!bs) {

        error_report("usb-msd: drive property not set");

        return -1;

    }



    

    bdrv_detach_dev(bs, &s->dev.qdev);

    s->conf.bs = NULL;



    if (!s->serial) {

        

        dinfo = drive_get_by_blockdev(bs);

        if (*dinfo->serial) {

            s->serial = strdup(dinfo->serial);

        }

    }

    if (s->serial) {

        usb_desc_set_string(dev, STR_SERIALNUMBER, s->serial);

    }



    usb_desc_init(dev);

    scsi_bus_new(&s->bus, &s->dev.qdev, &usb_msd_scsi_info);

    s->scsi_dev = scsi_bus_legacy_add_drive(&s->bus, bs, 0, !!s->removable,

                                            s->conf.bootindex);

    if (!s->scsi_dev) {

        return -1;

    }

    s->bus.qbus.allow_hotplug = 0;

    usb_msd_handle_reset(dev);



    if (bdrv_key_required(bs)) {

        if (cur_mon) {

            monitor_read_bdrv_key_start(cur_mon, bs, usb_msd_password_cb, s);

            s->dev.auto_attach = 0;

        } else {

            autostart = 0;

        }

    }



    return 0;

}