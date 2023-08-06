int ff_put_wav_header(AVIOContext *pb, AVCodecContext *enc)

{

    int bps, blkalign, bytespersec;

    int hdrsize = 18;

    int waveformatextensible;

    uint8_t temp[256];

    uint8_t *riff_extradata= temp;

    uint8_t *riff_extradata_start= temp;



    if(!enc->codec_tag || enc->codec_tag > 0xffff)

        return -1;

    waveformatextensible =   (enc->channels > 2 && enc->channel_layout)

                          || enc->sample_rate > 48000

                          || av_get_bits_per_sample(enc->codec_id) > 16;



    if (waveformatextensible) {

        avio_wl16(pb, 0xfffe);

    } else {

        avio_wl16(pb, enc->codec_tag);

    }

    avio_wl16(pb, enc->channels);

    avio_wl32(pb, enc->sample_rate);

    if (enc->codec_id == CODEC_ID_MP2 || enc->codec_id == CODEC_ID_MP3 || enc->codec_id == CODEC_ID_GSM_MS) {

        bps = 0;

    } else if (enc->codec_id == CODEC_ID_ADPCM_G726) {

        bps = 4;

    } else {

        if (!(bps = av_get_bits_per_sample(enc->codec_id)))

            bps = 16; 

    }

    if(bps != enc->bits_per_coded_sample && enc->bits_per_coded_sample){

        av_log(enc, AV_LOG_WARNING, "requested bits_per_coded_sample (%d) and actually stored (%d) differ\n", enc->bits_per_coded_sample, bps);

    }



    if (enc->codec_id == CODEC_ID_MP2 || enc->codec_id == CODEC_ID_MP3) {

        blkalign = enc->frame_size; 

        

    } else if (enc->codec_id == CODEC_ID_AC3) {

            blkalign = 3840; 

    } else if (enc->codec_id == CODEC_ID_ADPCM_G726) { 

        blkalign = 1;

    } else if (enc->block_align != 0) { 

        blkalign = enc->block_align;

    } else

        blkalign = enc->channels*bps >> 3;

    if (enc->codec_id == CODEC_ID_PCM_U8 ||

        enc->codec_id == CODEC_ID_PCM_S24LE ||

        enc->codec_id == CODEC_ID_PCM_S32LE ||

        enc->codec_id == CODEC_ID_PCM_F32LE ||

        enc->codec_id == CODEC_ID_PCM_F64LE ||

        enc->codec_id == CODEC_ID_PCM_S16LE) {

        bytespersec = enc->sample_rate * blkalign;

    } else {

        bytespersec = enc->bit_rate / 8;

    }

    avio_wl32(pb, bytespersec); 

    avio_wl16(pb, blkalign); 

    avio_wl16(pb, bps); 

    if (enc->codec_id == CODEC_ID_MP3) {

        hdrsize += 12;

        bytestream_put_le16(&riff_extradata, 1);    

        bytestream_put_le32(&riff_extradata, 2);    

        bytestream_put_le16(&riff_extradata, 1152); 

        bytestream_put_le16(&riff_extradata, 1);    

        bytestream_put_le16(&riff_extradata, 1393); 

    } else if (enc->codec_id == CODEC_ID_MP2) {

        hdrsize += 22;

        bytestream_put_le16(&riff_extradata, 2);                          

        bytestream_put_le32(&riff_extradata, enc->bit_rate);              

        bytestream_put_le16(&riff_extradata, enc->channels == 2 ? 1 : 8); 

        bytestream_put_le16(&riff_extradata, 0);                          

        bytestream_put_le16(&riff_extradata, 1);                          

        bytestream_put_le16(&riff_extradata, 16);                         

        bytestream_put_le32(&riff_extradata, 0);                          

        bytestream_put_le32(&riff_extradata, 0);                          

    } else if (enc->codec_id == CODEC_ID_GSM_MS || enc->codec_id == CODEC_ID_ADPCM_IMA_WAV) {

        hdrsize += 2;

        bytestream_put_le16(&riff_extradata, enc->frame_size); 

    } else if(enc->extradata_size){

        riff_extradata_start= enc->extradata;

        riff_extradata= enc->extradata + enc->extradata_size;

        hdrsize += enc->extradata_size;

    } else if (!waveformatextensible){

        hdrsize -= 2;

    }

    if(waveformatextensible) {                                    

        hdrsize += 22;

        avio_wl16(pb, riff_extradata - riff_extradata_start + 22); 

        avio_wl16(pb, enc->bits_per_coded_sample);                 

        avio_wl32(pb, enc->channel_layout);                        

        avio_wl32(pb, enc->codec_tag);                             

        avio_wl32(pb, 0x00100000);

        avio_wl32(pb, 0xAA000080);

        avio_wl32(pb, 0x719B3800);

    } else if(riff_extradata - riff_extradata_start) {

        avio_wl16(pb, riff_extradata - riff_extradata_start);

    }

    avio_write(pb, riff_extradata_start, riff_extradata - riff_extradata_start);

    if(hdrsize&1){

        hdrsize++;

        avio_w8(pb, 0);

    }



    return hdrsize;

}