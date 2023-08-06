static int virtio_rng_load_device(VirtIODevice *vdev, QEMUFile *f,

                                  int version_id)

{

    

    virtio_rng_process(VIRTIO_RNG(vdev));



    return 0;

}