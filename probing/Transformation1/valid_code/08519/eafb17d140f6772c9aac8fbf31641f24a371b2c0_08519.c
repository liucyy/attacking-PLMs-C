static int h264_handle_packet(AVFormatContext *ctx,
                              PayloadContext *data,
                              AVStream *st,
                              AVPacket * pkt,
                              uint32_t * timestamp,
                              const uint8_t * buf,
                              int len, int flags)
{
    uint8_t nal = buf[0];
    uint8_t type = (nal & 0x1f);
    int result= 0;
    uint8_t start_sequence[]= {0, 0, 1};
#ifdef DEBUG
    assert(data);
    assert(data->cookie == MAGIC_COOKIE);
#endif
    assert(buf);
    if (type >= 1 && type <= 23)
        type = 1;              
    switch (type) {
    case 0:                    
        result= -1;
        break;
    case 1:
        av_new_packet(pkt, len+sizeof(start_sequence));
        memcpy(pkt->data, start_sequence, sizeof(start_sequence));
        memcpy(pkt->data+sizeof(start_sequence), buf, len);
#ifdef DEBUG
        data->packet_types_received[nal & 0x1f]++;
#endif
        break;
    case 24:                   
        
        buf++;
        len--;
        
        {
            int pass= 0;
            int total_length= 0;
            uint8_t *dst= NULL;
            for(pass= 0; pass<2; pass++) {
                const uint8_t *src= buf;
                int src_len= len;
                do {
                    uint16_t nal_size = AV_RB16(src); 
                    
                    src += 2;
                    src_len -= 2;
                    if (nal_size <= src_len) {
                        if(pass==0) {
                            
                            total_length+= sizeof(start_sequence)+nal_size;
                        } else {
                            
                            assert(dst);
                            memcpy(dst, start_sequence, sizeof(start_sequence));
                            dst+= sizeof(start_sequence);
                            memcpy(dst, src, nal_size);
#ifdef DEBUG
                            data->packet_types_received[*src & 0x1f]++;
#endif
                            dst+= nal_size;
                        }
                    } else {
                        av_log(ctx, AV_LOG_ERROR,
                               "nal size exceeds length: %d %d\n", nal_size, src_len);
                    }
                    
                    src += nal_size;
                    src_len -= nal_size;
                    if (src_len < 0)
                        av_log(ctx, AV_LOG_ERROR,
                               "Consumed more bytes than we got! (%d)\n", src_len);
                } while (src_len > 2);      
                if(pass==0) {
                    
                    av_new_packet(pkt, total_length);
                    dst= pkt->data;
                } else {
                    assert(dst-pkt->data==total_length);
                }
            }
        }
        break;
    case 25:                   
    case 26:                   
    case 27:                   
    case 29:                   
        av_log(ctx, AV_LOG_ERROR,
               "Unhandled type (%d) (See RFC for implementation details\n",
               type);
        result= -1;
        break;
    case 28:                   
        buf++;
        len--;                  
        {
            
            uint8_t fu_indicator = nal;
            uint8_t fu_header = *buf;   
            uint8_t start_bit = fu_header >> 7;

            uint8_t nal_type = (fu_header & 0x1f);
            uint8_t reconstructed_nal;
            
            reconstructed_nal = fu_indicator & (0xe0);  
            reconstructed_nal |= nal_type;
            
            buf++;
            len--;
#ifdef DEBUG
            if (start_bit)
                data->packet_types_received[nal_type]++;
#endif
            if(start_bit) {
                
                av_new_packet(pkt, sizeof(start_sequence)+sizeof(nal)+len);
                memcpy(pkt->data, start_sequence, sizeof(start_sequence));
                pkt->data[sizeof(start_sequence)]= reconstructed_nal;
                memcpy(pkt->data+sizeof(start_sequence)+sizeof(nal), buf, len);
            } else {
                av_new_packet(pkt, len);
                memcpy(pkt->data, buf, len);
            }
        }
        break;
    case 30:                   
    case 31:                   
    default:
        av_log(ctx, AV_LOG_ERROR, "Undefined type (%d)", type);
        result= -1;
        break;
    }
    return result;
}