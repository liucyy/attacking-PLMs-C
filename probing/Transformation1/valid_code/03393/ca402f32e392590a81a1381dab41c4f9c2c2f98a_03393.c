void ff_get_wav_header(AVIOContext *pb, AVCodecContext *codec, int size)

{

    int id;



    id = avio_rl16(pb);

    codec->codec_type = AVMEDIA_TYPE_AUDIO;

    codec->codec_tag = id;

    codec->channels = avio_rl16(pb);

    codec->sample_rate = avio_rl32(pb);

    codec->bit_rate = avio_rl32(pb) * 8;

    codec->block_align = avio_rl16(pb);

    if (size == 14) {  

        codec->bits_per_coded_sample = 8;

    }else

        codec->bits_per_coded_sample = avio_rl16(pb);

    if (size >= 18) {  

        int cbSize = avio_rl16(pb); 

        size -= 18;

        cbSize = FFMIN(size, cbSize);

        if (cbSize >= 22 && id == 0xfffe) { 

            codec->bits_per_coded_sample = avio_rl16(pb);

            codec->channel_layout = avio_rl32(pb); 

            id = avio_rl32(pb); 

            avio_skip(pb, 12); 

            cbSize -= 22;

            size -= 22;

        }

        codec->extradata_size = cbSize;

        if (cbSize > 0) {

            codec->extradata = av_mallocz(codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);

            avio_read(pb, codec->extradata, codec->extradata_size);

            size -= cbSize;

        }



        

        if (size > 0)

            avio_skip(pb, size);

    }

    codec->codec_id = ff_wav_codec_get_id(id, codec->bits_per_coded_sample);

    if (codec->codec_id == CODEC_ID_AAC_LATM) {

        

        codec->channels    = 0;

        codec->sample_rate = 0;

    }

}