void isa_register_portio_list(ISADevice *dev, uint16_t start,

                              const MemoryRegionPortio *pio_start,

                              void *opaque, const char *name)

{

    PortioList piolist;



    

    isa_init_ioport(dev, start);



    

    portio_list_init(&piolist, OBJECT(dev), pio_start, opaque, name);

    portio_list_add(&piolist, isabus->address_space_io, start);

}