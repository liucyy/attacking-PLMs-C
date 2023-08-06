static bool scsi_block_is_passthrough(SCSIDiskState *s, uint8_t *buf)

{

    switch (buf[0]) {

    case VERIFY_10:

    case VERIFY_12:

    case VERIFY_16:

        

        if ((buf[1] & 6) != 2) {

            return false;

        }

        break;



    case READ_6:

    case READ_10:

    case READ_12:

    case READ_16:

    case WRITE_6:

    case WRITE_10:

    case WRITE_12:

    case WRITE_16:

    case WRITE_VERIFY_10:

    case WRITE_VERIFY_12:

    case WRITE_VERIFY_16:

        

        if (s->qdev.type != TYPE_ROM) {

            return false;

        }

        break;



    default:

        break;

    }



    return true;

}