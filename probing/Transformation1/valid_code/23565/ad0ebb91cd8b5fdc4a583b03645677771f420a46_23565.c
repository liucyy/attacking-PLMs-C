VIOsPAPRBus *spapr_vio_bus_init(void)

{

    VIOsPAPRBus *bus;

    BusState *qbus;

    DeviceState *dev;



    

    dev = qdev_create(NULL, "spapr-vio-bridge");

    qdev_init_nofail(dev);



    



    qbus = qbus_create(TYPE_SPAPR_VIO_BUS, dev, "spapr-vio");

    bus = DO_UPCAST(VIOsPAPRBus, bus, qbus);

    bus->next_reg = 0x1000;



    

    spapr_register_hypercall(H_VIO_SIGNAL, h_vio_signal);



    

    spapr_register_hypercall(H_PUT_TCE, h_put_tce);



    

    spapr_register_hypercall(H_REG_CRQ, h_reg_crq);

    spapr_register_hypercall(H_FREE_CRQ, h_free_crq);

    spapr_register_hypercall(H_SEND_CRQ, h_send_crq);

    spapr_register_hypercall(H_ENABLE_CRQ, h_enable_crq);



    

    spapr_rtas_register("ibm,set-tce-bypass", rtas_set_tce_bypass);

    spapr_rtas_register("quiesce", rtas_quiesce);



    return bus;

}