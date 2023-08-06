static int rtmp_server_handshake(URLContext *s, RTMPContext *rt)

{

    uint8_t buffer[RTMP_HANDSHAKE_PACKET_SIZE];

    uint32_t hs_epoch;

    uint32_t hs_my_epoch;

    uint8_t hs_c1[RTMP_HANDSHAKE_PACKET_SIZE];

    uint8_t hs_s1[RTMP_HANDSHAKE_PACKET_SIZE];

    uint32_t zeroes;

    uint32_t temp       = 0;

    int randomidx       = 0;

    int inoutsize       = 0;

    int ret;



    inoutsize = ffurl_read_complete(rt->stream, buffer, 1);       

    if (inoutsize <= 0) {

        av_log(s, AV_LOG_ERROR, "Unable to read handshake\n");

        return AVERROR(EIO);

    }

    

    if (buffer[0] != 3) {

        av_log(s, AV_LOG_ERROR, "RTMP protocol version mismatch\n");

        return AVERROR(EIO);

    }

    if (ffurl_write(rt->stream, buffer, 1) <= 0) {                 

        av_log(s, AV_LOG_ERROR,

               "Unable to write answer - RTMP S0\n");

        return AVERROR(EIO);

    }

    

    ret = rtmp_receive_hs_packet(rt, &hs_epoch, &zeroes, hs_c1,

                                 RTMP_HANDSHAKE_PACKET_SIZE);

    if (ret) {

        av_log(s, AV_LOG_ERROR, "RTMP Handshake C1 Error\n");

        return ret;

    }

    if (zeroes)

        av_log(s, AV_LOG_WARNING, "Erroneous C1 Message zero != 0\n");

    

    

    hs_my_epoch = hs_epoch;

    

    for (randomidx = 0; randomidx < (RTMP_HANDSHAKE_PACKET_SIZE);

         randomidx += 4)

        AV_WB32(hs_s1 + 8 + randomidx, av_get_random_seed());



    ret = rtmp_send_hs_packet(rt, hs_my_epoch, 0, hs_s1,

                              RTMP_HANDSHAKE_PACKET_SIZE);

    if (ret) {

        av_log(s, AV_LOG_ERROR, "RTMP Handshake S1 Error\n");

        return ret;

    }

    

    ret = rtmp_send_hs_packet(rt, hs_epoch, 0, hs_c1,

                              RTMP_HANDSHAKE_PACKET_SIZE);

    if (ret) {

        av_log(s, AV_LOG_ERROR, "RTMP Handshake S2 Error\n");

        return ret;

    }

    

    ret = rtmp_receive_hs_packet(rt, &temp, &zeroes, buffer,

                                 RTMP_HANDSHAKE_PACKET_SIZE);

    if (ret) {

        av_log(s, AV_LOG_ERROR, "RTMP Handshake C2 Error\n");

        return ret;

    }

    if (temp != hs_my_epoch)

        av_log(s, AV_LOG_WARNING,

               "Erroneous C2 Message epoch does not match up with C1 epoch\n");

    if (memcmp(buffer + 8, hs_s1 + 8,

               RTMP_HANDSHAKE_PACKET_SIZE - 8))

        av_log(s, AV_LOG_WARNING,

               "Erroneous C2 Message random does not match up\n");



    return 0;

}