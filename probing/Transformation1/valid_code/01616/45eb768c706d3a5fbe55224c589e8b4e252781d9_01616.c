static void pci_init_mask_bridge(PCIDevice *d)

{

    

    memset(d->wmask + PCI_PRIMARY_BUS, 0xff, 4);



    

    d->wmask[PCI_IO_BASE] = PCI_IO_RANGE_MASK & 0xff;

    d->wmask[PCI_IO_LIMIT] = PCI_IO_RANGE_MASK & 0xff;

    pci_set_word(d->wmask + PCI_MEMORY_BASE,

                 PCI_MEMORY_RANGE_MASK & 0xffff);

    pci_set_word(d->wmask + PCI_MEMORY_LIMIT,

                 PCI_MEMORY_RANGE_MASK & 0xffff);

    pci_set_word(d->wmask + PCI_PREF_MEMORY_BASE,

                 PCI_PREF_RANGE_MASK & 0xffff);

    pci_set_word(d->wmask + PCI_PREF_MEMORY_LIMIT,

                 PCI_PREF_RANGE_MASK & 0xffff);



    

    memset(d->wmask + PCI_PREF_BASE_UPPER32, 0xff, 8);



    

    d->config[PCI_IO_BASE] |= PCI_IO_RANGE_TYPE_16;

    d->config[PCI_IO_LIMIT] |= PCI_IO_RANGE_TYPE_16;

    pci_word_test_and_set_mask(d->config + PCI_PREF_MEMORY_BASE,

                               PCI_PREF_RANGE_TYPE_64);

    pci_word_test_and_set_mask(d->config + PCI_PREF_MEMORY_LIMIT,

                               PCI_PREF_RANGE_TYPE_64);





#define  PCI_BRIDGE_CTL_VGA_16BIT	0x10	

#define  PCI_BRIDGE_CTL_DISCARD		0x100	

#define  PCI_BRIDGE_CTL_SEC_DISCARD	0x200	

#define  PCI_BRIDGE_CTL_DISCARD_STATUS	0x400	

#define  PCI_BRIDGE_CTL_DISCARD_SERR	0x800	



    pci_set_word(d->wmask + PCI_BRIDGE_CONTROL,

                 PCI_BRIDGE_CTL_PARITY |

                 PCI_BRIDGE_CTL_SERR |

                 PCI_BRIDGE_CTL_ISA |

                 PCI_BRIDGE_CTL_VGA |

                 PCI_BRIDGE_CTL_VGA_16BIT |

                 PCI_BRIDGE_CTL_MASTER_ABORT |

                 PCI_BRIDGE_CTL_BUS_RESET |

                 PCI_BRIDGE_CTL_FAST_BACK |

                 PCI_BRIDGE_CTL_DISCARD |

                 PCI_BRIDGE_CTL_SEC_DISCARD |

                 PCI_BRIDGE_CTL_DISCARD_SERR);

    

    pci_set_word(d->w1cmask + PCI_BRIDGE_CONTROL,

                 PCI_BRIDGE_CTL_DISCARD_STATUS);

    d->cmask[PCI_IO_BASE] |= PCI_IO_RANGE_TYPE_MASK;

    d->cmask[PCI_IO_LIMIT] |= PCI_IO_RANGE_TYPE_MASK;

    pci_word_test_and_set_mask(d->cmask + PCI_PREF_MEMORY_BASE,

                               PCI_PREF_RANGE_TYPE_MASK);

    pci_word_test_and_set_mask(d->cmask + PCI_PREF_MEMORY_LIMIT,

                               PCI_PREF_RANGE_TYPE_MASK);

}