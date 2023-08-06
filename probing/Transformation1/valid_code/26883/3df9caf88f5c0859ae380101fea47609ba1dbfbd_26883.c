static void scsi_cancel_io(SCSIRequest *req)

{

    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);



    DPRINTF("Cancel tag=0x%x\n", req->tag);

    if (r->req.aiocb) {

        bdrv_aio_cancel(r->req.aiocb);



        

        scsi_req_unref(&r->req);

    }

    r->req.aiocb = NULL;

}