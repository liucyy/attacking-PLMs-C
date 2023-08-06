static void ccw_init(MachineState *machine)

{

    int ret;

    VirtualCssBus *css_bus;

    DeviceState *dev;



    s390_sclp_init();

    s390_memory_init(machine->ram_size);



    

    css_bus = virtual_css_bus_init();

    s390_init_ipl_dev(machine->kernel_filename, machine->kernel_cmdline,

                      machine->initrd_filename, "s390-ccw.img", true);

    s390_flic_init();



    dev = qdev_create(NULL, TYPE_S390_PCI_HOST_BRIDGE);

    object_property_add_child(qdev_get_machine(), TYPE_S390_PCI_HOST_BRIDGE,

                              OBJECT(dev), NULL);

    qdev_init_nofail(dev);



    

    virtio_ccw_register_hcalls();



    

    s390_init_cpus(machine->cpu_model);



    if (kvm_enabled()) {

        kvm_s390_enable_css_support(s390_cpu_addr2state(0));

    }

    

    ret = css_create_css_image(VIRTUAL_CSSID, true);

    assert(ret == 0);



    

    s390_create_virtio_net(BUS(css_bus), "virtio-net-ccw");



    

    register_savevm(NULL, "todclock", 0, 1,

                    gtod_save, gtod_load, kvm_state);

}