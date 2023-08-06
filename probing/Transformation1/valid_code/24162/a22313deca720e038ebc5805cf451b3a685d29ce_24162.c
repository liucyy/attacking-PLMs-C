static void vfio_platform_eoi(VFIODevice *vbasedev)

{

    VFIOINTp *intp;

    VFIOPlatformDevice *vdev =

        container_of(vbasedev, VFIOPlatformDevice, vbasedev);



    qemu_mutex_lock(&vdev->intp_mutex);

    QLIST_FOREACH(intp, &vdev->intp_list, next) {

        if (intp->state == VFIO_IRQ_ACTIVE) {

            trace_vfio_platform_eoi(intp->pin,

                                event_notifier_get_fd(&intp->interrupt));

            intp->state = VFIO_IRQ_INACTIVE;



            

            qemu_set_irq(intp->qemuirq, 0);



            if (intp->flags & VFIO_IRQ_INFO_AUTOMASKED) {

                

                vfio_unmask_single_irqindex(vbasedev, intp->pin);

            }



            

            break;

        }

    }

    

    if (!QSIMPLEQ_EMPTY(&vdev->pending_intp_queue)) {

        intp = QSIMPLEQ_FIRST(&vdev->pending_intp_queue);

        vfio_intp_inject_pending_lockheld(intp);

        QSIMPLEQ_REMOVE_HEAD(&vdev->pending_intp_queue, pqnext);

    }

    qemu_mutex_unlock(&vdev->intp_mutex);

}