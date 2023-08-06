static int qcow2_save_vmstate(BlockDriverState *bs, QEMUIOVector *qiov,

                              int64_t pos)

{

    BDRVQcowState *s = bs->opaque;

    int64_t total_sectors = bs->total_sectors;

    int growable = bs->growable;


    int ret;



    BLKDBG_EVENT(bs->file, BLKDBG_VMSTATE_SAVE);

    bs->growable = 1;


    ret = bdrv_pwritev(bs, qcow2_vm_state_offset(s) + pos, qiov);

    bs->growable = growable;




    

    bs->total_sectors = total_sectors;



    return ret;

}