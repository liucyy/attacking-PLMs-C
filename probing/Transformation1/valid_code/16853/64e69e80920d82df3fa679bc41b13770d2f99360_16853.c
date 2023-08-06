iscsi_aio_cancel(BlockDriverAIOCB *blockacb)

{

    IscsiAIOCB *acb = (IscsiAIOCB *)blockacb;

    IscsiLun *iscsilun = acb->iscsilun;



    acb->common.cb(acb->common.opaque, -ECANCELED);

    acb->canceled = 1;



    

    iscsi_task_mgmt_abort_task_async(iscsilun->iscsi, acb->task,

                                     iscsi_abort_task_cb, NULL);



    

    iscsi_scsi_task_cancel(iscsilun->iscsi, acb->task);

}