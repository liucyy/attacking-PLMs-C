int ff_isom_write_avcc(AVIOContext *pb, const uint8_t *data, int len)

{

    if (len > 6) {

        

        if (AV_RB32(data) == 0x00000001 ||

            AV_RB24(data) == 0x000001) {

            uint8_t *buf=NULL, *end, *start;

            uint32_t sps_size=0, pps_size=0;

            uint8_t *sps=0, *pps=0;



            int ret = ff_avc_parse_nal_units_buf(data, &buf, &len);

            if (ret < 0)

                return ret;

            start = buf;

            end = buf + len;



            

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



            avio_w8(pb, 1); 

            avio_w8(pb, sps[1]); 

            avio_w8(pb, sps[2]); 

            avio_w8(pb, sps[3]); 

            avio_w8(pb, 0xff); 

            avio_w8(pb, 0xe1); 



            avio_wb16(pb, sps_size);

            avio_write(pb, sps, sps_size);

            avio_w8(pb, 1); 

            avio_wb16(pb, pps_size);

            avio_write(pb, pps, pps_size);

            av_free(start);

        } else {

            avio_write(pb, data, len);

        }

    }

    return 0;

}