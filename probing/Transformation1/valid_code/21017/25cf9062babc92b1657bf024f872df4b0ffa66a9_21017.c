static int mov_write_video_tag(ByteIOContext *pb, MOVTrack* track)

{

    int pos = url_ftell(pb);

    char compressor_name[32];

    int tag;



    put_be32(pb, 0); 



    tag = track->enc->codec_tag;

    if (!tag)

    tag = codec_get_tag(codec_movvideo_tags, track->enc->codec_id);

    

    if (!tag)

	tag = codec_get_tag(codec_bmp_tags, track->enc->codec_id);

    put_le32(pb, tag); 



    put_be32(pb, 0); 

    put_be16(pb, 0); 

    put_be16(pb, 1); 



    put_be16(pb, 0); 

    put_be16(pb, 0); 

    put_tag(pb, "FFMP"); 

    if(track->enc->codec_id == CODEC_ID_RAWVIDEO) {

        put_be32(pb, 0); 

        put_be32(pb, 0x400); 

    } else {

        put_be32(pb, 0x200); 

        put_be32(pb, 0x200); 

    }

    put_be16(pb, track->enc->width); 

    put_be16(pb, track->enc->height); 

    put_be32(pb, 0x00480000); 

    put_be32(pb, 0x00480000); 

    put_be32(pb, 0); 

    put_be16(pb, 1); 

    

    memset(compressor_name,0,32);

    if (track->enc->codec->name)

        strncpy(compressor_name,track->enc->codec->name,31);

    put_byte(pb, FFMAX(strlen(compressor_name),32) );

    put_buffer(pb, compressor_name, 31);

    

    put_be16(pb, 0x18); 

    put_be16(pb, 0xffff); 

    if(track->enc->codec_id == CODEC_ID_MPEG4)

        mov_write_esds_tag(pb, track);

    else if(track->enc->codec_id == CODEC_ID_H263)

        mov_write_d263_tag(pb);

    else if(track->enc->codec_id == CODEC_ID_SVQ3)

        mov_write_svq3_tag(pb);    



    return updateSize (pb, pos);

}