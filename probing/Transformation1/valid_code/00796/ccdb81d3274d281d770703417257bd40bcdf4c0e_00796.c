static void imx_fec_reset(DeviceState *d)

{

    IMXFECState *s = IMX_FEC(d);



    

    s->eir = 0;

    s->eimr = 0;

    s->rx_enabled = 0;

    s->ecr = 0;

    s->mscr = 0;

    s->mibc = 0xc0000000;

    s->rcr = 0x05ee0001;

    s->tcr = 0;

    s->tfwr = 0;

    s->frsr = 0x500;

    s->miigsk_cfgr = 0;

    s->miigsk_enr = 0x6;



    

    phy_reset(s);

}