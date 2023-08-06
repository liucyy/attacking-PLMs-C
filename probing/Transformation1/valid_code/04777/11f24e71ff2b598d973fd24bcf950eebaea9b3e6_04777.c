static void output_packet(OutputFile *of, AVPacket *pkt, OutputStream *ost)

{

    int ret = 0;



    

    if (ost->nb_bitstream_filters) {

        int idx;




        ret = av_bsf_send_packet(ost->bsf_ctx[0], pkt);

        if (ret < 0)

            goto finish;



        idx = 1;

        while (idx) {

            

            ret = av_bsf_receive_packet(ost->bsf_ctx[idx - 1], pkt);

            

            if (!(ost->bsf_extradata_updated[idx - 1] & 1)) {

                ret = avcodec_parameters_copy(ost->st->codecpar, ost->bsf_ctx[idx - 1]->par_out);

                if (ret < 0)

                    goto finish;

                ost->bsf_extradata_updated[idx - 1] |= 1;

            }

            if (ret == AVERROR(EAGAIN)) {

                ret = 0;

                idx--;

                continue;

            } else if (ret < 0)

                goto finish;



            

            if (idx < ost->nb_bitstream_filters) {

                

                if (!(ost->bsf_extradata_updated[idx] & 2)) {

                    ret = avcodec_parameters_copy(ost->bsf_ctx[idx]->par_out, ost->bsf_ctx[idx - 1]->par_out);

                    if (ret < 0)

                        goto finish;

                    ost->bsf_extradata_updated[idx] |= 2;

                }

                ret = av_bsf_send_packet(ost->bsf_ctx[idx], pkt);

                if (ret < 0)

                    goto finish;

                idx++;

            } else

                write_packet(of, pkt, ost);

        }

    } else

        write_packet(of, pkt, ost);



finish:

    if (ret < 0 && ret != AVERROR_EOF) {

        av_log(NULL, AV_LOG_ERROR, "Error applying bitstream filters to an output "

               "packet for stream #%d:%d.\n", ost->file_index, ost->index);

        if(exit_on_error)

            exit_program(1);

    }

}