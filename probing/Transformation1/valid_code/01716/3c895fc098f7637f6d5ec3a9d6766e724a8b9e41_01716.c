static int rm_write_video(AVFormatContext *s, const uint8_t *buf, int size)

{

    RMContext *rm = s->priv_data;

    ByteIOContext *pb = &s->pb;

    StreamInfo *stream = rm->video_stream;

    int key_frame = stream->enc->coded_frame->key_frame;



    



    

#if 1

    write_packet_header(s, stream, size + 7, key_frame);

    

    put_byte(pb, 0x81); 

    

    if (key_frame) {

        put_byte(pb, 0x81); 

    } else {

        put_byte(pb, 0x01); 

    }

    put_be16(pb, 0x4000 | (size)); 

    put_be16(pb, 0x4000 | (size));              

#else

    

    write_packet_header(s, size + 6);

    put_byte(pb, 0xc0); 

    put_be16(pb, 0x4000 | size); 

    put_be16(pb, 0x4000 + packet_number * 126); 

#endif

    put_byte(pb, stream->nb_frames & 0xff); 

    

    put_buffer(pb, buf, size);

    put_flush_packet(pb);



    stream->nb_frames++;

    return 0;

}