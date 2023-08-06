static void virtqueue_map_iovec(struct iovec *sg, hwaddr *addr,

                                unsigned int *num_sg, unsigned int max_size,

                                int is_write)

{

    unsigned int i;

    hwaddr len;



    

    

#ifdef NDEBUG

#error building with NDEBUG is not supported

#endif

    assert(*num_sg <= max_size);



    for (i = 0; i < *num_sg; i++) {

        len = sg[i].iov_len;

        sg[i].iov_base = cpu_physical_memory_map(addr[i], &len, is_write);

        if (!sg[i].iov_base) {

            error_report("virtio: error trying to map MMIO memory");

            exit(1);

        }

        if (len != sg[i].iov_len) {

            error_report("virtio: unexpected memory split");

            exit(1);

        }

    }

}