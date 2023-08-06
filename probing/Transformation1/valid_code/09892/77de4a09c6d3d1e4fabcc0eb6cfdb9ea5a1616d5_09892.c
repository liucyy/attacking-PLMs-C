static void usb_msd_realize_storage(USBDevice *dev, Error **errp)

{

    MSDState *s = DO_UPCAST(MSDState, dev, dev);

    BlockDriverState *bs = s->conf.bs;

    SCSIDevice *scsi_dev;

    Error *err = NULL;



    if (!bs) {

        error_setg(errp, "drive property not set");

        return;

    }



    blkconf_serial(&s->conf, &dev->serial);



    

    bdrv_detach_dev(bs, &s->dev.qdev);

    s->conf.bs = NULL;



    usb_desc_create_serial(dev);

    usb_desc_init(dev);

    scsi_bus_new(&s->bus, sizeof(s->bus), DEVICE(dev),

                 &usb_msd_scsi_info_storage, NULL);

    scsi_dev = scsi_bus_legacy_add_drive(&s->bus, bs, 0, !!s->removable,

                                         s->conf.bootindex, dev->serial,

                                         &err);

    if (!scsi_dev) {

        error_propagate(errp, err);

        return;

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

}