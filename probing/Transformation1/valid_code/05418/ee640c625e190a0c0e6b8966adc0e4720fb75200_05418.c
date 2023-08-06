static int vfio_msix_setup(VFIOPCIDevice *vdev, int pos, Error **errp)

{

    int ret;



    vdev->msix->pending = g_malloc0(BITS_TO_LONGS(vdev->msix->entries) *

                                    sizeof(unsigned long));

    ret = msix_init(&vdev->pdev, vdev->msix->entries,

                    vdev->bars[vdev->msix->table_bar].region.mem,

                    vdev->msix->table_bar, vdev->msix->table_offset,

                    vdev->bars[vdev->msix->pba_bar].region.mem,

                    vdev->msix->pba_bar, vdev->msix->pba_offset, pos);

    if (ret < 0) {

        if (ret == -ENOTSUP) {

            return 0;

        }

        error_setg(errp, "msix_init failed");

        return ret;

    }



    

    memory_region_set_enabled(&vdev->pdev.msix_pba_mmio, false);



    return 0;

}