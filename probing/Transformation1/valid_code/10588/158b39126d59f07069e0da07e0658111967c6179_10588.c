static int aac_sync(uint64_t state, AACAC3ParseContext *hdr_info,

        int *need_next_header, int *new_frame_start)

{

    GetBitContext bits;

    int size, rdb, ch, sr;

    union {

        uint64_t u64;

        uint8_t  u8[8];

    } tmp;



    tmp.u64 = be2me_64(state);

    init_get_bits(&bits, tmp.u8+8-AAC_HEADER_SIZE, AAC_HEADER_SIZE * 8);



    if(get_bits(&bits, 12) != 0xfff)

        return 0;



    skip_bits1(&bits);          

    skip_bits(&bits, 2);        

    skip_bits1(&bits);          

    skip_bits(&bits, 2);        

    sr = get_bits(&bits, 4);    

    if(!ff_mpeg4audio_sample_rates[sr])

        return 0;

    skip_bits1(&bits);          

    ch = get_bits(&bits, 3);    

    if(!ff_mpeg4audio_channels[ch])

        return 0;

    skip_bits1(&bits);          

    skip_bits1(&bits);          



    

    skip_bits1(&bits);          

    skip_bits1(&bits);          

    size = get_bits(&bits, 13); 

    if(size < AAC_HEADER_SIZE)

        return 0;



    skip_bits(&bits, 11);       

    rdb = get_bits(&bits, 2);   



    hdr_info->channels = ff_mpeg4audio_channels[ch];

    hdr_info->sample_rate = ff_mpeg4audio_sample_rates[sr];

    hdr_info->samples = (rdb + 1) * 1024;

    hdr_info->bit_rate = size * 8 * hdr_info->sample_rate / hdr_info->samples;



    *need_next_header = 0;

    *new_frame_start  = 1;

    return size;

}