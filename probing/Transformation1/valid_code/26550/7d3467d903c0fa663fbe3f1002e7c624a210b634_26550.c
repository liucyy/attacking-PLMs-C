static void usb_msd_realize_storage(USBDevice *dev, Error **errp)

{

    MSDState *s = USB_STORAGE_DEV(dev);

    BlockBackend *blk = s->conf.blk;

    SCSIDevice *scsi_dev;

    Error *err = NULL;



    if (!blk) {

        error_setg(errp, "drive property not set");

        return;

    }



    bdrv_add_key(blk_bs(blk), NULL, &err);

    if (err) {

        if (monitor_cur_is_qmp()) {

            error_propagate(errp, err);

            return;

        }

        error_free(err);

        err = NULL;

        if (cur_mon) {

            monitor_read_bdrv_key_start(cur_mon, blk_bs(blk),

                                        usb_msd_password_cb, s);

            s->dev.auto_attach = 0;

        } else {

            autostart = 0;

        }

    }



    blkconf_serial(&s->conf, &dev->serial);

    blkconf_blocksizes(&s->conf);



    

    blk_detach_dev(blk, &s->dev.qdev);

    s->conf.blk = NULL;



    usb_desc_create_serial(dev);

    usb_desc_init(dev);

    scsi_bus_new(&s->bus, sizeof(s->bus), DEVICE(dev),

                 &usb_msd_scsi_info_storage, NULL);

    scsi_dev = scsi_bus_legacy_add_drive(&s->bus, blk, 0, !!s->removable,

                                         s->conf.bootindex, dev->serial,

                                         &err);

    if (!scsi_dev) {

        error_propagate(errp, err);

        return;

    }

    usb_msd_handle_reset(dev);

    s->scsi_dev = scsi_dev;

}