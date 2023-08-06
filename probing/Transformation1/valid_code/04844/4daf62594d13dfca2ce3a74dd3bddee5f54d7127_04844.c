static int xen_pt_pci_config_access_check(PCIDevice *d, uint32_t addr, int len)

{

    

    if (addr >= 0xFF) {

        XEN_PT_ERR(d, "Failed to access register with offset exceeding 0xFF. "

                   "(addr: 0x%02x, len: %d)\n", addr, len);

        return -1;

    }



    

    if ((len != 1) && (len != 2) && (len != 4)) {

        XEN_PT_ERR(d, "Failed to access register with invalid access length. "

                   "(addr: 0x%02x, len: %d)\n", addr, len);

        return -1;

    }



    

    if (addr & (len - 1)) {

        XEN_PT_ERR(d, "Failed to access register with invalid access size "

                   "alignment. (addr: 0x%02x, len: %d)\n", addr, len);

        return -1;

    }



    return 0;

}