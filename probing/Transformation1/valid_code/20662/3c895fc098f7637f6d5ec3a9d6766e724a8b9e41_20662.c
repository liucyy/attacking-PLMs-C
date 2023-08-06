static void put_payload_header(

                                AVFormatContext *s,

                                ASFStream       *stream,

                                int             presentation_time,

                                int             m_obj_size,

                                int             m_obj_offset,

                                int             payload_len

            )

{

    ASFContext *asf = s->priv_data;

    ByteIOContext *pb = &asf->pb;

    int val;

    

    val = stream->num;

    if (s->streams[val - 1]->codec.coded_frame->key_frame)

        val |= ASF_PL_FLAG_KEY_FRAME;

    put_byte(pb, val);

        

    put_byte(pb, stream->seq);  

    put_le32(pb, m_obj_offset); 

         

    

    

    

    

    

    put_byte(pb, ASF_PAYLOAD_REPLICATED_DATA_LENGTH);



    put_le32(pb, m_obj_size);       

    put_le32(pb, presentation_time);

    

    if (asf->multi_payloads_present){

        put_le16(pb, payload_len);   

    }

}