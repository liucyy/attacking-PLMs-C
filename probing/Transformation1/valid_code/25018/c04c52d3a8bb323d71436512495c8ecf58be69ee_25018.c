static int amf_parse_object(AVFormatContext *s, AVStream *astream, AVStream *vstream, const char *key, unsigned int max_pos, int depth) {

    AVCodecContext *acodec, *vcodec;

    ByteIOContext *ioc;

    AMFDataType amf_type;

    char str_val[256];

    double num_val;



    num_val = 0;

    ioc = s->pb;



    amf_type = get_byte(ioc);



    switch(amf_type) {

        case AMF_DATA_TYPE_NUMBER:

            num_val = av_int2dbl(get_be64(ioc)); break;

        case AMF_DATA_TYPE_BOOL:

            num_val = get_byte(ioc); break;

        case AMF_DATA_TYPE_STRING:

            if(amf_get_string(ioc, str_val, sizeof(str_val)) < 0)

                return -1;

            break;

        case AMF_DATA_TYPE_OBJECT: {

            unsigned int keylen;



            while(url_ftell(ioc) < max_pos - 2 && (keylen = get_be16(ioc))) {

                url_fskip(ioc, keylen); 

                if(amf_parse_object(s, NULL, NULL, NULL, max_pos, depth + 1) < 0)

                    return -1; 

            }

            if(get_byte(ioc) != AMF_END_OF_OBJECT)

                return -1;

        }

            break;

        case AMF_DATA_TYPE_NULL:

        case AMF_DATA_TYPE_UNDEFINED:

        case AMF_DATA_TYPE_UNSUPPORTED:

            break; 

        case AMF_DATA_TYPE_MIXEDARRAY:

            url_fskip(ioc, 4); 

            while(url_ftell(ioc) < max_pos - 2 && amf_get_string(ioc, str_val, sizeof(str_val)) > 0) {

                

                if(amf_parse_object(s, astream, vstream, str_val, max_pos, depth + 1) < 0)

                    return -1;

            }

            if(get_byte(ioc) != AMF_END_OF_OBJECT)

                return -1;

            break;

        case AMF_DATA_TYPE_ARRAY: {

            unsigned int arraylen, i;



            arraylen = get_be32(ioc);

            for(i = 0; i < arraylen && url_ftell(ioc) < max_pos - 1; i++) {

                if(amf_parse_object(s, NULL, NULL, NULL, max_pos, depth + 1) < 0)

                    return -1; 

            }

        }

            break;

        case AMF_DATA_TYPE_DATE:

            url_fskip(ioc, 8 + 2); 

            break;

        default: 

            return -1;

    }



    if(depth == 1 && key) { 

        acodec = astream ? astream->codec : NULL;

        vcodec = vstream ? vstream->codec : NULL;



        if(amf_type == AMF_DATA_TYPE_BOOL) {

            if(!strcmp(key, "stereo") && acodec) acodec->channels = num_val > 0 ? 2 : 1;

        } else if(amf_type == AMF_DATA_TYPE_NUMBER) {

            if(!strcmp(key, "duration")) s->duration = num_val * AV_TIME_BASE;





            else if(!strcmp(key, "audiocodecid") && acodec) flv_set_audio_codec(s, astream, (int)num_val << FLV_AUDIO_CODECID_OFFSET);

            else if(!strcmp(key, "videocodecid") && vcodec) flv_set_video_codec(s, vstream, (int)num_val);

            else if(!strcmp(key, "audiosamplesize") && acodec && num_val >= 0) {

                acodec->bits_per_sample = num_val;

                

                if(num_val == 8 && (acodec->codec_id == CODEC_ID_PCM_S16BE || acodec->codec_id == CODEC_ID_PCM_S16LE))

                    acodec->codec_id = CODEC_ID_PCM_S8;

            }

            else if(!strcmp(key, "audiosamplerate") && acodec && num_val >= 0) {

                

                if (!acodec->sample_rate) {

                    switch((int)num_val) {

                        case 44000: acodec->sample_rate = 44100  ; break;

                        case 22000: acodec->sample_rate = 22050  ; break;

                        case 11000: acodec->sample_rate = 11025  ; break;

                        case 5000 : acodec->sample_rate = 5512   ; break;

                        default   : acodec->sample_rate = num_val;

                    }

                }

            }

        }

    }



    return 0;

}