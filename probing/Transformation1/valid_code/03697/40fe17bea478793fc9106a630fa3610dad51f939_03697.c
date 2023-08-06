static void ahci_init_d2h(AHCIDevice *ad)

{

    IDEState *ide_state = &ad->port.ifs[0];

    AHCIPortRegs *pr = &ad->port_regs;



    if (ad->init_d2h_sent) {

        return;

    }



    if (ahci_write_fis_d2h(ad)) {

        ad->init_d2h_sent = true;

        

        pr->sig = (ide_state->hcyl << 24) |

            (ide_state->lcyl << 16) |

            (ide_state->sector << 8) |

            (ide_state->nsector & 0xFF);

    }

}