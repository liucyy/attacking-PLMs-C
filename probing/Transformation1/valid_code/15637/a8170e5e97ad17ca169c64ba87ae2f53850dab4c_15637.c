static void assigned_dev_msix_mmio_write(void *opaque, target_phys_addr_t addr,

                                         uint64_t val, unsigned size)

{

    AssignedDevice *adev = opaque;

    PCIDevice *pdev = &adev->dev;

    uint16_t ctrl;

    MSIXTableEntry orig;

    int i = addr >> 4;



    if (i >= adev->msix_max) {

        return; 

    }



    ctrl = pci_get_word(pdev->config + pdev->msix_cap + PCI_MSIX_FLAGS);



    DEBUG("write to MSI-X table offset 0x%lx, val 0x%lx\n", addr, val);



    if (ctrl & PCI_MSIX_FLAGS_ENABLE) {

        orig = adev->msix_table[i];

    }



    memcpy((uint8_t *)adev->msix_table + addr, &val, size);



    if (ctrl & PCI_MSIX_FLAGS_ENABLE) {

        MSIXTableEntry *entry = &adev->msix_table[i];



        if (!assigned_dev_msix_masked(&orig) &&

            assigned_dev_msix_masked(entry)) {

            

        } else if (assigned_dev_msix_masked(&orig) &&

                   !assigned_dev_msix_masked(entry)) {

            

            if (i >= adev->msi_virq_nr || adev->msi_virq[i] < 0) {

                

                assigned_dev_update_msix(pdev);

                return;

            } else {

                

                MSIMessage msg;

                int ret;



                msg.address = entry->addr_lo |

                    ((uint64_t)entry->addr_hi << 32);

                msg.data = entry->data;



                ret = kvm_irqchip_update_msi_route(kvm_state,

                                                   adev->msi_virq[i], msg);

                if (ret) {

                    error_report("Error updating irq routing entry (%d)", ret);

                }

            }

        }

    }

}