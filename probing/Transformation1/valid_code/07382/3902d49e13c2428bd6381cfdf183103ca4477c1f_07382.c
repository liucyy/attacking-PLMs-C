static int virtio_rng_load(QEMUFile *f, void *opaque, int version_id)

{

    VirtIORNG *vrng = opaque;

    VirtIODevice *vdev = VIRTIO_DEVICE(vrng);



    if (version_id != 1) {

        return -EINVAL;

    }

    virtio_load(vdev, f, version_id);



    

    virtio_rng_process(vrng);



    return 0;

}