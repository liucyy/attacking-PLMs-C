static int alac_set_info(ALACContext *alac)

{

    const unsigned char *ptr = alac->avctx->extradata;



    ptr += 4; 

    ptr += 4; 

    ptr += 4; 



    if(AV_RB32(ptr) >= UINT_MAX/4){

        av_log(alac->avctx, AV_LOG_ERROR, "setinfo_max_samples_per_frame too large\n");

        return -1;

    }



    

    alac->setinfo_max_samples_per_frame = bytestream_get_be32(&ptr);

    ptr++;                          

    alac->setinfo_sample_size           = *ptr++;

    alac->setinfo_rice_historymult      = *ptr++;

    alac->setinfo_rice_initialhistory   = *ptr++;

    alac->setinfo_rice_kmodifier        = *ptr++;

    alac->numchannels                   = *ptr++;

    bytestream_get_be16(&ptr);      

    bytestream_get_be32(&ptr);      

    bytestream_get_be32(&ptr);      

    bytestream_get_be32(&ptr);      



    return 0;

}