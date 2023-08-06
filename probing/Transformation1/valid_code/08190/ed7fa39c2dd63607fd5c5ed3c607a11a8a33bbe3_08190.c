static int mov_write_avcc_tag(ByteIOContext *pb, MOVTrack *track)

{

    offset_t pos = url_ftell(pb);



    put_be32(pb, 0);

    put_tag(pb, "avcC");

    if (track->vosLen > 6) {

        

        if (AV_RB32(track->vosData) == 0x00000001) {

            uint8_t *buf, *end;

            uint32_t sps_size=0, pps_size=0;

            uint8_t *sps=0, *pps=0;



            avc_parse_nal_units(&track->vosData, &track->vosLen);

            buf = track->vosData;

            end = track->vosData + track->vosLen;



            

            while (buf < end) {

                unsigned int size;

                uint8_t nal_type;

                size = AV_RB32(buf);

                nal_type = buf[4] & 0x1f;

                if (nal_type == 7) { 

                    sps = buf + 4;

                    sps_size = size;

                } else if (nal_type == 8) { 

                    pps = buf + 4;

                    pps_size = size;

                }

                buf += size + 4;

            }

            assert(sps);

            assert(pps);



            put_byte(pb, 1); 

            put_byte(pb, sps[1]); 

            put_byte(pb, sps[2]); 

            put_byte(pb, sps[3]); 

            put_byte(pb, 0xff); 

            put_byte(pb, 0xe1); 



            put_be16(pb, sps_size);

            put_buffer(pb, sps, sps_size);

            put_byte(pb, 1); 

            put_be16(pb, pps_size);

            put_buffer(pb, pps, pps_size);

        } else {

            put_buffer(pb, track->vosData, track->vosLen);

        }

    }

    return updateSize(pb, pos);

}