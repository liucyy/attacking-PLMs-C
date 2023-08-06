int ff_put_wav_header(AVFormatContext *s, AVIOContext *pb,

                      AVCodecParameters *par, int flags)

{

    int bps, blkalign, bytespersec, frame_size;

    int hdrsize;

    int64_t hdrstart = avio_tell(pb);

    int waveformatextensible;

    uint8_t temp[256];

    uint8_t *riff_extradata       = temp;

    uint8_t *riff_extradata_start = temp;



    if (!par->codec_tag || par->codec_tag > 0xffff)

        return -1;



    

    frame_size = av_get_audio_frame_duration2(par, par->block_align);



    waveformatextensible = (par->channels > 2 && par->channel_layout) ||



                           par->sample_rate > 48000 ||

                           par->codec_id == AV_CODEC_ID_EAC3 ||

                           av_get_bits_per_sample(par->codec_id) > 16;



    if (waveformatextensible)

        avio_wl16(pb, 0xfffe);

    else

        avio_wl16(pb, par->codec_tag);



    avio_wl16(pb, par->channels);

    avio_wl32(pb, par->sample_rate);

    if (par->codec_id == AV_CODEC_ID_ATRAC3 ||

        par->codec_id == AV_CODEC_ID_G723_1 ||

        par->codec_id == AV_CODEC_ID_MP2    ||

        par->codec_id == AV_CODEC_ID_MP3    ||

        par->codec_id == AV_CODEC_ID_GSM_MS) {

        bps = 0;

    } else {

        if (!(bps = av_get_bits_per_sample(par->codec_id))) {

            if (par->bits_per_coded_sample)

                bps = par->bits_per_coded_sample;

            else

                bps = 16;  

        }

    }

    if (bps != par->bits_per_coded_sample && par->bits_per_coded_sample) {

        av_log(s, AV_LOG_WARNING,

               "requested bits_per_coded_sample (%d) "

               "and actually stored (%d) differ\n",

               par->bits_per_coded_sample, bps);

    }



    if (par->codec_id == AV_CODEC_ID_MP2) {

        blkalign = (144 * par->bit_rate - 1)/par->sample_rate + 1;

    } else if (par->codec_id == AV_CODEC_ID_MP3) {

        blkalign = 576 * (par->sample_rate <= (24000 + 32000)/2 ? 1 : 2);

    } else if (par->codec_id == AV_CODEC_ID_AC3) {

        blkalign = 3840;                

    } else if (par->codec_id == AV_CODEC_ID_AAC) {

        blkalign = 768 * par->channels; 

    } else if (par->codec_id == AV_CODEC_ID_G723_1) {

        blkalign = 24;

    } else if (par->block_align != 0) { 

        blkalign = par->block_align;

    } else

        blkalign = bps * par->channels / av_gcd(8, bps);

    if (par->codec_id == AV_CODEC_ID_PCM_U8 ||

        par->codec_id == AV_CODEC_ID_PCM_S24LE ||

        par->codec_id == AV_CODEC_ID_PCM_S32LE ||

        par->codec_id == AV_CODEC_ID_PCM_F32LE ||

        par->codec_id == AV_CODEC_ID_PCM_F64LE ||

        par->codec_id == AV_CODEC_ID_PCM_S16LE) {

        bytespersec = par->sample_rate * blkalign;

    } else if (par->codec_id == AV_CODEC_ID_G723_1) {

        bytespersec = 800;

    } else {

        bytespersec = par->bit_rate / 8;

    }

    avio_wl32(pb, bytespersec); 

    avio_wl16(pb, blkalign);    

    avio_wl16(pb, bps);         

    if (par->codec_id == AV_CODEC_ID_MP3) {

        bytestream_put_le16(&riff_extradata, 1);    

        bytestream_put_le32(&riff_extradata, 2);    

        bytestream_put_le16(&riff_extradata, 1152); 

        bytestream_put_le16(&riff_extradata, 1);    

        bytestream_put_le16(&riff_extradata, 1393); 

    } else if (par->codec_id == AV_CODEC_ID_MP2) {

        

        bytestream_put_le16(&riff_extradata, 2);

        

        bytestream_put_le32(&riff_extradata, par->bit_rate);

        

        bytestream_put_le16(&riff_extradata, par->channels == 2 ? 1 : 8);

        

        bytestream_put_le16(&riff_extradata, 0);

        

        bytestream_put_le16(&riff_extradata, 1);

        

        bytestream_put_le16(&riff_extradata, 16);

        

        bytestream_put_le32(&riff_extradata, 0);

        

        bytestream_put_le32(&riff_extradata, 0);

    } else if (par->codec_id == AV_CODEC_ID_G723_1) {

        bytestream_put_le32(&riff_extradata, 0x9ace0002); 

        bytestream_put_le32(&riff_extradata, 0xaea2f732);

        bytestream_put_le16(&riff_extradata, 0xacde);

    } else if (par->codec_id == AV_CODEC_ID_GSM_MS ||

               par->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV) {

        

        bytestream_put_le16(&riff_extradata, frame_size);

    } else if (par->extradata_size) {

        riff_extradata_start = par->extradata;

        riff_extradata       = par->extradata + par->extradata_size;

    }

    

    if (waveformatextensible) {

        int write_channel_mask = !(flags & FF_PUT_WAV_HEADER_SKIP_CHANNELMASK) &&

                                 (s->strict_std_compliance < FF_COMPLIANCE_NORMAL ||

                                  par->channel_layout < 0x40000);

        

        avio_wl16(pb, riff_extradata - riff_extradata_start + 22);

        

        avio_wl16(pb, bps);

        

        avio_wl32(pb, write_channel_mask ? par->channel_layout : 0);

        

        if (par->codec_id == AV_CODEC_ID_EAC3) {

            ff_put_guid(pb, ff_get_codec_guid(par->codec_id, ff_codec_wav_guids));

        } else {

        avio_wl32(pb, par->codec_tag);

        avio_wl32(pb, 0x00100000);

        avio_wl32(pb, 0xAA000080);

        avio_wl32(pb, 0x719B3800);

        }

    } else if ((flags & FF_PUT_WAV_HEADER_FORCE_WAVEFORMATEX) ||

               par->codec_tag != 0x0001  ||

               riff_extradata - riff_extradata_start) {

        

        avio_wl16(pb, riff_extradata - riff_extradata_start); 

    } 

    avio_write(pb, riff_extradata_start, riff_extradata - riff_extradata_start);

    hdrsize = avio_tell(pb) - hdrstart;

    if (hdrsize & 1) {

        hdrsize++;

        avio_w8(pb, 0);

    }



    return hdrsize;

}