static int coroutine_fn qed_aio_write_cow(QEDAIOCB *acb)

{

    BDRVQEDState *s = acb_to_s(acb);

    uint64_t start, len, offset;

    int ret;



    

    start = qed_start_of_cluster(s, acb->cur_pos);

    len = qed_offset_into_cluster(s, acb->cur_pos);



    trace_qed_aio_write_prefill(s, acb, start, len, acb->cur_cluster);

    ret = qed_copy_from_backing_file(s, start, len, acb->cur_cluster);

    if (ret < 0) {

        return ret;

    }



    

    start = acb->cur_pos + acb->cur_qiov.size;

    len = qed_start_of_cluster(s, start + s->header.cluster_size - 1) - start;

    offset = acb->cur_cluster +

             qed_offset_into_cluster(s, acb->cur_pos) +

             acb->cur_qiov.size;



    trace_qed_aio_write_postfill(s, acb, start, len, offset);

    ret = qed_copy_from_backing_file(s, start, len, offset);

    if (ret < 0) {

        return ret;

    }



    ret = qed_aio_write_main(acb);

    if (ret < 0) {

        return ret;

    }



    if (s->bs->backing) {

        

        ret = bdrv_co_flush(s->bs->file->bs);

        if (ret < 0) {

            return ret;

        }

    }



    return 0;

}