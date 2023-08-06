static int spapr_vio_busdev_init(DeviceState *qdev)

{

    VIOsPAPRDevice *dev = (VIOsPAPRDevice *)qdev;

    VIOsPAPRDeviceClass *pc = VIO_SPAPR_DEVICE_GET_CLASS(dev);

    uint32_t liobn;

    char *id;



    if (dev->reg != -1) {

        

        VIOsPAPRDevice *other = reg_conflict(dev);



        if (other) {

            fprintf(stderr, "vio: %s and %s devices conflict at address %#x\n",

                    object_get_typename(OBJECT(qdev)),

                    object_get_typename(OBJECT(&other->qdev)),

                    dev->reg);

            return -1;

        }

    } else {

        

        VIOsPAPRBus *bus = DO_UPCAST(VIOsPAPRBus, bus, dev->qdev.parent_bus);



        do {

            dev->reg = bus->next_reg++;

        } while (reg_conflict(dev));

    }



    

    if (!dev->qdev.id) {

        id = vio_format_dev_name(dev);

        if (!id) {

            return -1;

        }

        dev->qdev.id = id;

    }



    dev->qirq = spapr_allocate_msi(dev->vio_irq_num, &dev->vio_irq_num);

    if (!dev->qirq) {

        return -1;

    }



    liobn = SPAPR_VIO_BASE_LIOBN | dev->reg;

    dev->dma = spapr_tce_new_dma_context(liobn, pc->rtce_window_size);



    return pc->init(dev);

}