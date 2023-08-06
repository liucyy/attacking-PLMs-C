static int msix_add_config(struct PCIDevice *pdev, unsigned short nentries,

                           unsigned bar_nr, unsigned bar_size)

{

    int config_offset;

    uint8_t *config;

    uint32_t new_size;



    if (nentries < 1 || nentries > PCI_MSIX_FLAGS_QSIZE + 1)

        return -EINVAL;

    if (bar_size > 0x80000000)

        return -ENOSPC;



    

    if (!bar_size) {

        new_size = MSIX_PAGE_SIZE;

    } else if (bar_size < MSIX_PAGE_SIZE) {

        bar_size = MSIX_PAGE_SIZE;

        new_size = MSIX_PAGE_SIZE * 2;

    } else {

        new_size = bar_size * 2;

    }



    pdev->msix_bar_size = new_size;

    config_offset = pci_add_capability(pdev, PCI_CAP_ID_MSIX,

                                       0, MSIX_CAP_LENGTH);

    if (config_offset < 0)

        return config_offset;

    config = pdev->config + config_offset;



    pci_set_word(config + PCI_MSIX_FLAGS, nentries - 1);

    

    pci_set_long(config + PCI_MSIX_TABLE, bar_size | bar_nr);

    

    pci_set_long(config + PCI_MSIX_PBA, (bar_size + MSIX_PAGE_PENDING) |

                 bar_nr);

    pdev->msix_cap = config_offset;

    

    pdev->wmask[config_offset + MSIX_CONTROL_OFFSET] |= MSIX_ENABLE_MASK |

	    MSIX_MASKALL_MASK;

    pdev->msix_function_masked = true;

    return 0;

}