static bool cmd_set_features(IDEState *s, uint8_t cmd)

{

    uint16_t *identify_data;



    if (!s->bs) {

        ide_abort_command(s);

        return true;

    }



    

    switch (s->feature) {

    case 0x02: 

        bdrv_set_enable_write_cache(s->bs, true);

        identify_data = (uint16_t *)s->identify_data;

        put_le16(identify_data + 85, (1 << 14) | (1 << 5) | 1);

        return true;

    case 0x82: 

        bdrv_set_enable_write_cache(s->bs, false);

        identify_data = (uint16_t *)s->identify_data;

        put_le16(identify_data + 85, (1 << 14) | 1);

        ide_flush_cache(s);

        return false;

    case 0xcc: 

    case 0x66: 

    case 0xaa: 

    case 0x55: 

    case 0x05: 

    case 0x85: 

    case 0x69: 

    case 0x67: 

    case 0x96: 

    case 0x9a: 

    case 0x42: 

    case 0xc2: 

        return true;

    case 0x03: 

        {

            uint8_t val = s->nsector & 0x07;

            identify_data = (uint16_t *)s->identify_data;



            switch (s->nsector >> 3) {

            case 0x00: 

            case 0x01: 

                put_le16(identify_data + 62, 0x07);

                put_le16(identify_data + 63, 0x07);

                put_le16(identify_data + 88, 0x3f);

                break;

            case 0x02: 

                put_le16(identify_data + 62, 0x07 | (1 << (val + 8)));

                put_le16(identify_data + 63, 0x07);

                put_le16(identify_data + 88, 0x3f);

                break;

            case 0x04: 

                put_le16(identify_data + 62, 0x07);

                put_le16(identify_data + 63, 0x07 | (1 << (val + 8)));

                put_le16(identify_data + 88, 0x3f);

                break;

            case 0x08: 

                put_le16(identify_data + 62, 0x07);

                put_le16(identify_data + 63, 0x07);

                put_le16(identify_data + 88, 0x3f | (1 << (val + 8)));

                break;

            default:

                goto abort_cmd;

            }

            return true;

        }

    }



abort_cmd:

    ide_abort_command(s);

    return true;

}