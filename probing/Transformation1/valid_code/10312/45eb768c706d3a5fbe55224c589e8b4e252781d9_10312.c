void pcie_port_init_reg(PCIDevice *d)

{

    

    pci_set_word(d->config + PCI_STATUS, 0);

    pci_set_word(d->config + PCI_SEC_STATUS, 0);



    

#define  PCI_BRIDGE_CTL_VGA_16BIT       0x10    

    pci_set_word(d->wmask + PCI_BRIDGE_CONTROL,

                 PCI_BRIDGE_CTL_PARITY |

                 PCI_BRIDGE_CTL_ISA |

                 PCI_BRIDGE_CTL_VGA |

                 PCI_BRIDGE_CTL_VGA_16BIT | 

                 PCI_BRIDGE_CTL_SERR |

                 PCI_BRIDGE_CTL_BUS_RESET);

}