static int vhdx_update_header(BlockDriverState *bs, BDRVVHDXState *s,

                              bool generate_data_write_guid, MSGUID *log_guid)

{

    int ret = 0;

    int hdr_idx = 0;

    uint64_t header_offset = VHDX_HEADER1_OFFSET;



    VHDXHeader *active_header;

    VHDXHeader *inactive_header;



    

    if (s->curr_header == 0) {

        hdr_idx = 1;

        header_offset = VHDX_HEADER2_OFFSET;

    }



    active_header   = s->headers[s->curr_header];

    inactive_header = s->headers[hdr_idx];



    inactive_header->sequence_number = active_header->sequence_number + 1;



    

    inactive_header->file_write_guid = s->session_guid;



    

    if (generate_data_write_guid) {

        vhdx_guid_generate(&inactive_header->data_write_guid);

    }



    

    if (log_guid) {

        inactive_header->log_guid = *log_guid;

    }



    vhdx_write_header(bs->file, inactive_header, header_offset, true);

    if (ret < 0) {

        goto exit;

    }

    s->curr_header = hdr_idx;



exit:

    return ret;

}