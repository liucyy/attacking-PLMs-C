static int scsi_disk_emulate_inquiry(SCSIRequest *req, uint8_t *outbuf)

{

    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, req->dev);

    int buflen = 0;



    if (req->cmd.buf[1] & 0x2) {

        

        BADF("optional INQUIRY command support request not implemented\n");

        return -1;

    }



    if (req->cmd.buf[1] & 0x1) {

        

        uint8_t page_code = req->cmd.buf[2];

        if (req->cmd.xfer < 4) {

            BADF("Error: Inquiry (EVPD[%02X]) buffer size %zd is "

                 "less than 4\n", page_code, req->cmd.xfer);

            return -1;

        }



        if (bdrv_get_type_hint(s->bs) == BDRV_TYPE_CDROM) {

            outbuf[buflen++] = 5;

        } else {

            outbuf[buflen++] = 0;

        }

        outbuf[buflen++] = page_code ; 

        outbuf[buflen++] = 0x00;



        switch (page_code) {

        case 0x00: 

            DPRINTF("Inquiry EVPD[Supported pages] "

                    "buffer size %zd\n", req->cmd.xfer);

            outbuf[buflen++] = 4;    

            outbuf[buflen++] = 0x00; 

            outbuf[buflen++] = 0x80; 

            outbuf[buflen++] = 0x83; 

            outbuf[buflen++] = 0xb0; 

            break;



        case 0x80: 

        {

            const char *serial = req->dev->conf.dinfo->serial ?

                req->dev->conf.dinfo->serial : "0";

            int l = strlen(serial);



            if (l > req->cmd.xfer)

                l = req->cmd.xfer;

            if (l > 20)

                l = 20;



            DPRINTF("Inquiry EVPD[Serial number] "

                    "buffer size %zd\n", req->cmd.xfer);

            outbuf[buflen++] = l;

            memcpy(outbuf+buflen, serial, l);

            buflen += l;

            break;

        }



        case 0x83: 

        {

            int max_len = 255 - 8;

            int id_len = strlen(bdrv_get_device_name(s->bs));



            if (id_len > max_len)

                id_len = max_len;

            DPRINTF("Inquiry EVPD[Device identification] "

                    "buffer size %zd\n", req->cmd.xfer);



            outbuf[buflen++] = 3 + id_len;

            outbuf[buflen++] = 0x2; 

            outbuf[buflen++] = 0;   

            outbuf[buflen++] = 0;   

            outbuf[buflen++] = id_len; 



            memcpy(outbuf+buflen, bdrv_get_device_name(s->bs), id_len);

            buflen += id_len;

            break;

        }

        case 0xb0: 

        {

            unsigned int min_io_size = s->qdev.conf.min_io_size >> 9;

            unsigned int opt_io_size = s->qdev.conf.opt_io_size >> 9;



            

            outbuf[3] = buflen = 0x3c;



            memset(outbuf + 4, 0, buflen - 4);



            

            outbuf[6] = (min_io_size >> 8) & 0xff;

            outbuf[7] = min_io_size & 0xff;



            

            outbuf[12] = (opt_io_size >> 24) & 0xff;

            outbuf[13] = (opt_io_size >> 16) & 0xff;

            outbuf[14] = (opt_io_size >> 8) & 0xff;

            outbuf[15] = opt_io_size & 0xff;

            break;

        }

        default:

            BADF("Error: unsupported Inquiry (EVPD[%02X]) "

                 "buffer size %zd\n", page_code, req->cmd.xfer);

            return -1;

        }

        

        return buflen;

    }



    

    if (req->cmd.buf[2] != 0) {

        BADF("Error: Inquiry (STANDARD) page or code "

             "is non-zero [%02X]\n", req->cmd.buf[2]);

        return -1;

    }



    

    if (req->cmd.xfer < 5) {

        BADF("Error: Inquiry (STANDARD) buffer size %zd "

             "is less than 5\n", req->cmd.xfer);

        return -1;

    }



    buflen = req->cmd.xfer;

    if (buflen > SCSI_MAX_INQUIRY_LEN)

        buflen = SCSI_MAX_INQUIRY_LEN;



    memset(outbuf, 0, buflen);



    if (req->lun || req->cmd.buf[1] >> 5) {

        outbuf[0] = 0x7f;	

        return buflen;

    }



    if (bdrv_get_type_hint(s->bs) == BDRV_TYPE_CDROM) {

        outbuf[0] = 5;

        outbuf[1] = 0x80;

        memcpy(&outbuf[16], "QEMU CD-ROM     ", 16);

    } else {

        outbuf[0] = 0;

        memcpy(&outbuf[16], "QEMU HARDDISK   ", 16);

    }

    memcpy(&outbuf[8], "QEMU    ", 8);

    memcpy(&outbuf[32], s->version ? s->version : QEMU_VERSION, 4);

    

    outbuf[2] = 5;

    outbuf[3] = 2; 



    if (buflen > 36) {

        outbuf[4] = buflen - 5; 

    } else {

        

        outbuf[4] = 36 - 5;

    }



    

    outbuf[7] = 0x10 | (req->bus->tcq ? 0x02 : 0);

    return buflen;

}