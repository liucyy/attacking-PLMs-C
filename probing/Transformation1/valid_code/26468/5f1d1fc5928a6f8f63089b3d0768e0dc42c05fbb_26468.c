int spapr_populate_vdevice(VIOsPAPRBus *bus, void *fdt)

{

    DeviceState *qdev, **qdevs;

    BusChild *kid;

    int i, num, ret = 0;



    

    num = 0;

    QTAILQ_FOREACH(kid, &bus->bus.children, sibling) {

        num++;

    }



    

    qdevs = g_malloc(sizeof(qdev) * num);

    num = 0;

    QTAILQ_FOREACH(kid, &bus->bus.children, sibling) {

        qdevs[num++] = kid->child;

    }



    

    qsort(qdevs, num, sizeof(qdev), compare_reg);



    

    for (i = num - 1; i >= 0; i--) {

        VIOsPAPRDevice *dev = (VIOsPAPRDevice *)(qdevs[i]);



        ret = vio_make_devnode(dev, fdt);



        if (ret < 0) {

            goto out;

        }

    }



    ret = 0;

out:

    free(qdevs);



    return ret;

}