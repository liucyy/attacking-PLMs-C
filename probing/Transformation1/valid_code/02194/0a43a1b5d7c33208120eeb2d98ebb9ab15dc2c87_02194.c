static int vhdx_parse_log(BlockDriverState *bs, BDRVVHDXState *s)

{

    int ret = 0;

    int i;

    VHDXHeader *hdr;



    hdr = s->headers[s->curr_header];



    

    for (i = 0; i < sizeof(hdr->log_guid.data4); i++) {

        ret |= hdr->log_guid.data4[i];

    }

    if (hdr->log_guid.data1 == 0 &&

        hdr->log_guid.data2 == 0 &&

        hdr->log_guid.data3 == 0 &&

        ret == 0) {

        goto exit;

    }



    

    if (hdr->log_version != 0) {

        ret = -EINVAL;

        goto exit;

    }



    if (hdr->log_length == 0) {

        goto exit;

    }



    

    ret = -ENOTSUP;



exit:

    return ret;

}