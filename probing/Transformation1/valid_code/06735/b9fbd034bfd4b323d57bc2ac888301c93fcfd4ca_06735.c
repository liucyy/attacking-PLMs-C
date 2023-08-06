static int xwma_read_header(AVFormatContext *s)

{

    int64_t size;

    int ret = 0;

    uint32_t dpds_table_size = 0;

    uint32_t *dpds_table = NULL;

    unsigned int tag;

    AVIOContext *pb = s->pb;

    AVStream *st;

    XWMAContext *xwma = s->priv_data;

    int i;



    



    

    tag = avio_rl32(pb);

    if (tag != MKTAG('R', 'I', 'F', 'F'))

        return -1;

    avio_rl32(pb); 

    tag = avio_rl32(pb);

    if (tag != MKTAG('X', 'W', 'M', 'A'))

        return -1;



    

    tag = avio_rl32(pb);

    if (tag != MKTAG('f', 'm', 't', ' '))

        return -1;

    size = avio_rl32(pb);

    st = avformat_new_stream(s, NULL);

    if (!st)

        return AVERROR(ENOMEM);



    ret = ff_get_wav_header(pb, st->codec, size, 0);

    if (ret < 0)

        return ret;

    st->need_parsing = AVSTREAM_PARSE_NONE;



    

    if (st->codec->codec_id != AV_CODEC_ID_WMAV2) {

        avpriv_request_sample(s, "Unexpected codec (tag 0x04%x; id %d)",

                              st->codec->codec_tag, st->codec->codec_id);

    } else {

        

        if (st->codec->extradata_size != 0) {

            

            avpriv_request_sample(s, "Unexpected extradata (%d bytes)",

                                  st->codec->extradata_size);

        } else {

            st->codec->extradata_size = 6;

            st->codec->extradata      = av_mallocz(6 + FF_INPUT_BUFFER_PADDING_SIZE);

            if (!st->codec->extradata)

                return AVERROR(ENOMEM);



            

            st->codec->extradata[4] = 31;

        }

    }



    if (!st->codec->channels) {

        av_log(s, AV_LOG_WARNING, "Invalid channel count: %d\n",

               st->codec->channels);

        return AVERROR_INVALIDDATA;

    }

    if (!st->codec->bits_per_coded_sample) {

        av_log(s, AV_LOG_WARNING, "Invalid bits_per_coded_sample: %d\n",

               st->codec->bits_per_coded_sample);

        return AVERROR_INVALIDDATA;

    }



    

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);



    

    for (;;) {

        if (pb->eof_reached) {

            ret = AVERROR_EOF;

            goto fail;

        }

        

        tag = avio_rl32(pb);

        size = avio_rl32(pb);

        if (tag == MKTAG('d', 'a', 't', 'a')) {

            

            break;

        } else if (tag == MKTAG('d','p','d','s')) {

            



            

            if (dpds_table) {

                av_log(s, AV_LOG_ERROR, "two dpds chunks present\n");

                ret = AVERROR_INVALIDDATA;

                goto fail;

            }



            

            if (size & 3) {  

                av_log(s, AV_LOG_WARNING,

                       "dpds chunk size %"PRId64" not divisible by 4\n", size);

            }

            dpds_table_size = size / 4;

            if (dpds_table_size == 0 || dpds_table_size >= INT_MAX / 4) {

                av_log(s, AV_LOG_ERROR,

                       "dpds chunk size %"PRId64" invalid\n", size);

                return AVERROR_INVALIDDATA;

            }



            

            dpds_table = av_malloc(dpds_table_size * sizeof(uint32_t));

            if (!dpds_table) {

                return AVERROR(ENOMEM);

            }



            for (i = 0; i < dpds_table_size; ++i) {

                dpds_table[i] = avio_rl32(pb);

                size -= 4;

            }

        }

        avio_skip(pb, size);

    }



    

    if (size < 0) {

        ret = AVERROR_INVALIDDATA;

        goto fail;

    }

    if (!size) {

        xwma->data_end = INT64_MAX;

    } else

        xwma->data_end = avio_tell(pb) + size;





    if (dpds_table && dpds_table_size) {

        int64_t cur_pos;

        const uint32_t bytes_per_sample

                = (st->codec->channels * st->codec->bits_per_coded_sample) >> 3;



        

        const uint64_t total_decoded_bytes = dpds_table[dpds_table_size - 1];



        if (!bytes_per_sample) {

            av_log(s, AV_LOG_ERROR,

                   "Invalid bits_per_coded_sample %d for %d channels\n",

                   st->codec->bits_per_coded_sample, st->codec->channels);

            ret = AVERROR_INVALIDDATA;

            goto fail;

        }



        st->duration = total_decoded_bytes / bytes_per_sample;



        

        cur_pos = avio_tell(pb);

        for (i = 0; i < dpds_table_size; ++i) {

            

            av_add_index_entry(st,

                               cur_pos + (i+1) * st->codec->block_align, 

                               dpds_table[i] / bytes_per_sample,         

                               st->codec->block_align,                   

                               0,                                        

                               AVINDEX_KEYFRAME);

        }

    } else if (st->codec->bit_rate) {

        

        st->duration = (size<<3) * st->codec->sample_rate / st->codec->bit_rate;

    }



fail:

    av_free(dpds_table);



    return ret;

}