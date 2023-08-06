VIOsPAPRDevice *spapr_vty_get_default(VIOsPAPRBus *bus)

{

    VIOsPAPRDevice *sdev, *selected;

    DeviceState *iter;



    



    selected = NULL;

    QTAILQ_FOREACH(iter, &bus->bus.children, sibling) {

        

        if (qdev_get_info(iter) != &spapr_vty_info.qdev) {

            continue;

        }



        sdev = DO_UPCAST(VIOsPAPRDevice, qdev, iter);



        

        if (!selected) {

            selected = sdev;

            continue;

        }



        

        if (sdev->reg < selected->reg) {

            selected = sdev;

        }

    }



    return selected;

}