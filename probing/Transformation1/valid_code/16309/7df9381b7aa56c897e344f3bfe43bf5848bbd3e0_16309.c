static void vfio_err_notifier_handler(void *opaque)

{

    VFIOPCIDevice *vdev = opaque;



    if (!event_notifier_test_and_clear(&vdev->err_notifier)) {

        return;

    }



    



    error_report("%s(%04x:%02x:%02x.%x) Unrecoverable error detected.  "

                 "Please collect any data possible and then kill the guest",

                 __func__, vdev->host.domain, vdev->host.bus,

                 vdev->host.slot, vdev->host.function);



    vm_stop(RUN_STATE_INTERNAL_ERROR);

}