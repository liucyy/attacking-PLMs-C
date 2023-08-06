static void ccw_init(MachineState *machine)

{

    int ret;

    VirtualCssBus *css_bus;



    s390_sclp_init();

    s390_memory_init(machine->ram_size);



    

    s390_init_cpus(machine);



    s390_flic_init();



    

    css_bus = virtual_css_bus_init();

    s390_init_ipl_dev(machine->kernel_filename, machine->kernel_cmdline,

                      machine->initrd_filename, "s390-ccw.img",

                      "s390-netboot.img", true);



    if (s390_has_feat(S390_FEAT_ZPCI)) {

        DeviceState *dev = qdev_create(NULL, TYPE_S390_PCI_HOST_BRIDGE);

        object_property_add_child(qdev_get_machine(),

                                  TYPE_S390_PCI_HOST_BRIDGE,

                                  OBJECT(dev), NULL);

        qdev_init_nofail(dev);

    }



    

    virtio_ccw_register_hcalls();



    s390_enable_css_support(s390_cpu_addr2state(0));

    

    if (css_bus->squash_mcss) {

        ret = css_create_css_image(0, true);

    } else {

        ret = css_create_css_image(VIRTUAL_CSSID, true);

    }

    assert(ret == 0);



    

    s390_create_virtio_net(BUS(css_bus), "virtio-net-ccw");



    

    register_savevm_live(NULL, "todclock", 0, 1, &savevm_gtod, NULL);

}