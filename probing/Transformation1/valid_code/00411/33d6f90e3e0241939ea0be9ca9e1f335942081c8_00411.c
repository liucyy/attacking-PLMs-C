static int http_send_data(HTTPContext *c)

{

    int len, ret;



    for(;;) {

        if (c->buffer_ptr >= c->buffer_end) {

            ret = http_prepare_data(c);

            if (ret < 0)

                return -1;

            else if (ret != 0)

                

                break;

        } else {

            if (c->is_packetized) {

                

                len = c->buffer_end - c->buffer_ptr;

                if (len < 4) {

                    

                fail1:

                    c->buffer_ptr = c->buffer_end;

                    return 0;

                }

                len = (c->buffer_ptr[0] << 24) |

                    (c->buffer_ptr[1] << 16) |

                    (c->buffer_ptr[2] << 8) |

                    (c->buffer_ptr[3]);

                if (len > (c->buffer_end - c->buffer_ptr))

                    goto fail1;

                if ((get_packet_send_clock(c) - get_server_clock(c)) > 0) {

                    

                    return 0;

                }



                c->data_count += len;

                update_datarate(&c->datarate, c->data_count);

                if (c->stream)

                    c->stream->bytes_served += len;



                if (c->rtp_protocol == RTSP_LOWER_TRANSPORT_TCP) {

                    

                    AVIOContext *pb;

                    int interleaved_index, size;

                    uint8_t header[4];

                    HTTPContext *rtsp_c;



                    rtsp_c = c->rtsp_c;

                    

                    if (!rtsp_c)

                        return -1;

                    

                    if (rtsp_c->state != RTSPSTATE_WAIT_REQUEST)

                        break;

                    if (avio_open_dyn_buf(&pb) < 0)

                        goto fail1;

                    interleaved_index = c->packet_stream_index * 2;

                    

                    if (c->buffer_ptr[1] == 200)

                        interleaved_index++;

                    

                    header[0] = '$';

                    header[1] = interleaved_index;

                    header[2] = len >> 8;

                    header[3] = len;

                    avio_write(pb, header, 4);

                    

                    c->buffer_ptr += 4;

                    avio_write(pb, c->buffer_ptr, len);

                    size = avio_close_dyn_buf(pb, &c->packet_buffer);

                    

                    rtsp_c->packet_buffer_ptr = c->packet_buffer;

                    rtsp_c->packet_buffer_end = c->packet_buffer + size;

                    c->buffer_ptr += len;



                    

                    len = send(rtsp_c->fd, rtsp_c->packet_buffer_ptr,

                                rtsp_c->packet_buffer_end - rtsp_c->packet_buffer_ptr, 0);

                    if (len > 0)

                        rtsp_c->packet_buffer_ptr += len;

                    if (rtsp_c->packet_buffer_ptr < rtsp_c->packet_buffer_end) {

                        

                        rtsp_c->state = RTSPSTATE_SEND_PACKET;

                        break;

                    } else

                        

                        av_freep(&c->packet_buffer);

                } else {

                    

                    c->buffer_ptr += 4;

                    ffurl_write(c->rtp_handles[c->packet_stream_index],

                                c->buffer_ptr, len);

                    c->buffer_ptr += len;

                    

                }

            } else {

                

                len = send(c->fd, c->buffer_ptr, c->buffer_end - c->buffer_ptr, 0);

                if (len < 0) {

                    if (ff_neterrno() != AVERROR(EAGAIN) &&

                        ff_neterrno() != AVERROR(EINTR))

                        

                        return -1;

                    else

                        return 0;

                } else

                    c->buffer_ptr += len;



                c->data_count += len;

                update_datarate(&c->datarate, c->data_count);

                if (c->stream)

                    c->stream->bytes_served += len;

                break;

            }

        }

    } 

    return 0;

}