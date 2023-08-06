static void tcx_init(target_phys_addr_t addr, int vram_size, int width,

                     int height, int depth)

{

    DeviceState *dev;

    SysBusDevice *s;



    dev = qdev_create(NULL, "SUNW,tcx");

    qdev_prop_set_taddr(dev, "addr", addr);

    qdev_prop_set_uint32(dev, "vram_size", vram_size);

    qdev_prop_set_uint16(dev, "width", width);

    qdev_prop_set_uint16(dev, "height", height);

    qdev_prop_set_uint16(dev, "depth", depth);

    qdev_init(dev);

    s = sysbus_from_qdev(dev);

    

    sysbus_mmio_map(s, 0, addr + 0x00800000ULL);

    

    sysbus_mmio_map(s, 1, addr + 0x00200000ULL);

    

    sysbus_mmio_map(s, 2, addr + 0x00700000ULL);

    

    sysbus_mmio_map(s, 3, addr + 0x00301000ULL);

    if (depth == 24) {

        

        sysbus_mmio_map(s, 4, addr + 0x02000000ULL);

        

        sysbus_mmio_map(s, 5, addr + 0x0a000000ULL);

    } else {

        

        sysbus_mmio_map(s, 4, addr + 0x00300000ULL);

    }

}