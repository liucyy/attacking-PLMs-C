static int qemu_rdma_broken_ipv6_kernel(Error **errp, struct ibv_context *verbs)

{

    struct ibv_port_attr port_attr;



    

#ifdef CONFIG_LINUX



    


        int num_devices, x;

        struct ibv_device ** dev_list = ibv_get_device_list(&num_devices);

        bool roce_found = false;

        bool ib_found = false;



        for (x = 0; x < num_devices; x++) {

            verbs = ibv_open_device(dev_list[x]);










            if (ibv_query_port(verbs, 1, &port_attr)) {

                ibv_close_device(verbs);

                ERROR(errp, "Could not query initial IB port");





            if (port_attr.link_layer == IBV_LINK_LAYER_INFINIBAND) {

                ib_found = true;

            } else if (port_attr.link_layer == IBV_LINK_LAYER_ETHERNET) {

                roce_found = true;




            ibv_close_device(verbs);






        if (roce_found) {

            if (ib_found) {

                fprintf(stderr, "WARN: migrations may fail:"

                                " IPv6 over RoCE / iWARP in linux"

                                " is broken. But since you appear to have a"

                                " mixed RoCE / IB environment, be sure to only"

                                " migrate over the IB fabric until the kernel "

                                " fixes the bug.\n");


                ERROR(errp, "You only have RoCE / iWARP devices in your systems"

                            " and your management software has specified '[::]'"

                            ", but IPv6 over RoCE / iWARP is not supported in Linux.");

                return -ENONET;





        return 0;




    



    

    if (ibv_query_port(verbs, 1, &port_attr)) {

        ERROR(errp, "Could not query initial IB port");





    if (port_attr.link_layer == IBV_LINK_LAYER_ETHERNET) {

        ERROR(errp, "Linux kernel's RoCE / iWARP does not support IPv6 "

                    "(but patches on linux-rdma in progress)");

        return -ENONET;




#endif



    return 0;