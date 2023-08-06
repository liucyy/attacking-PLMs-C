static void ahci_pci_enable(AHCIQState *ahci)

{

    uint8_t reg;



    start_ahci_device(ahci);



    switch (ahci->fingerprint) {

    case AHCI_INTEL_ICH9:

        

        reg = qpci_config_readb(ahci->dev, 0x92);

        reg |= 0x3F;

        qpci_config_writeb(ahci->dev, 0x92, reg);

        

        ASSERT_BIT_SET(qpci_config_readb(ahci->dev, 0x92), 0x3F);

        break;

    }



}