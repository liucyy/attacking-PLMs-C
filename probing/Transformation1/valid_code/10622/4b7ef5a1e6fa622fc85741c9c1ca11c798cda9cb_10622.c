static int parse_keyframes_index(AVFormatContext *s, AVIOContext *ioc, AVStream *vstream, int64_t max_pos) {

    unsigned int timeslen = 0, fileposlen = 0, i;

    char str_val[256];

    int64_t *times = NULL;

    int64_t *filepositions = NULL;

    int ret = AVERROR(ENOSYS);

    int64_t initial_pos = avio_tell(ioc);

    AVDictionaryEntry *creator = av_dict_get(s->metadata, "metadatacreator",

                                             NULL, 0);



    if (creator && !strcmp(creator->value, "MEGA")) {

        

        return 0;

    }



    if(vstream->nb_index_entries>0){

        av_log(s, AV_LOG_WARNING, "Skiping duplicate index\n");

        return 0;

    }



    while (avio_tell(ioc) < max_pos - 2 && amf_get_string(ioc, str_val, sizeof(str_val)) > 0) {

        int64_t** current_array;

        unsigned int arraylen;



        

        if (avio_r8(ioc) != AMF_DATA_TYPE_ARRAY)

            break;



        arraylen = avio_rb32(ioc);

        if(arraylen>>28)

            break;



        if       (!strcmp(KEYFRAMES_TIMESTAMP_TAG , str_val) && !times){

            current_array= &times;

            timeslen= arraylen;

        }else if (!strcmp(KEYFRAMES_BYTEOFFSET_TAG, str_val) && !filepositions){

            current_array= &filepositions;

            fileposlen= arraylen;

        }else 

            break;



        if (!(*current_array = av_mallocz(sizeof(**current_array) * arraylen))) {

            ret = AVERROR(ENOMEM);

            goto finish;

        }



        for (i = 0; i < arraylen && avio_tell(ioc) < max_pos - 1; i++) {

            if (avio_r8(ioc) != AMF_DATA_TYPE_NUMBER)

                goto finish;

            current_array[0][i] = av_int2dbl(avio_rb64(ioc));

        }

        if (times && filepositions) {

            

            

            ret = 0;

            break;

        }

    }



    if (timeslen == fileposlen) {

         for(i = 0; i < timeslen; i++)

             av_add_index_entry(vstream, filepositions[i], times[i]*1000, 0, 0, AVINDEX_KEYFRAME);

    } else

        av_log(s, AV_LOG_WARNING, "Invalid keyframes object, skipping.\n");



finish:

    av_freep(&times);

    av_freep(&filepositions);

    avio_seek(ioc, initial_pos, SEEK_SET);

    return ret;

}