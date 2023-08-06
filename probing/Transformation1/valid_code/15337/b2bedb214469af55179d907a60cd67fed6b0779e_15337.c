static void vt82c686b_write_config(PCIDevice * d, uint32_t address,

                                   uint32_t val, int len)

{

    VT82C686BState *vt686 = DO_UPCAST(VT82C686BState, dev, d);



    DPRINTF("vt82c686b_write_config  address 0x%x  val 0x%x len 0x%x \n",

           address, val, len);



    pci_default_write_config(d, address, val, len);

    if (address == 0x85) {  

        if (val & 0x2) {

            

            isa_unassign_ioport(0x3f0, 2);

            register_ioport_read(0x3f0, 2, 1, superio_ioport_readb,

                                 &vt686->superio_conf);

            register_ioport_write(0x3f0, 2, 1, superio_ioport_writeb,

                                  &vt686->superio_conf);

        } else {

            isa_unassign_ioport(0x3f0, 2);

        }

    }

}