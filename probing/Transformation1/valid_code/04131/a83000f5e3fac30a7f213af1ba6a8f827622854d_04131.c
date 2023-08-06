static void spapr_phb_reset(DeviceState *qdev)

{

    SysBusDevice *s = SYS_BUS_DEVICE(qdev);

    sPAPRPHBState *sphb = SPAPR_PCI_HOST_BRIDGE(s);



    

    spapr_tce_reset(sphb->tcet);

}