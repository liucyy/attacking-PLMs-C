pcie_cap_v1_fill(uint8_t *exp_cap, uint8_t port, uint8_t type, uint8_t version)

{

    

    pci_set_word(exp_cap + PCI_EXP_FLAGS,

                 ((type << PCI_EXP_FLAGS_TYPE_SHIFT) & PCI_EXP_FLAGS_TYPE) |

                 version);



    

    pci_set_long(exp_cap + PCI_EXP_DEVCAP, PCI_EXP_DEVCAP_RBER);



    pci_set_long(exp_cap + PCI_EXP_LNKCAP,

                 (port << PCI_EXP_LNKCAP_PN_SHIFT) |

                 PCI_EXP_LNKCAP_ASPMS_0S |

                 PCI_EXP_LNK_MLW_1 |

                 PCI_EXP_LNK_LS_25);



    pci_set_word(exp_cap + PCI_EXP_LNKSTA,

                 PCI_EXP_LNK_MLW_1 | PCI_EXP_LNK_LS_25 |PCI_EXP_LNKSTA_DLLLA);

}