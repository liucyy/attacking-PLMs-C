static int vhdx_log_write(BlockDriverState *bs, BDRVVHDXState *s,

                          void *data, uint32_t length, uint64_t offset)

{

    int ret = 0;

    void *buffer = NULL;

    void *merged_sector = NULL;

    void *data_tmp, *sector_write;

    unsigned int i;

    int sector_offset;

    uint32_t desc_sectors, sectors, total_length;

    uint32_t sectors_written = 0;

    uint32_t aligned_length;

    uint32_t leading_length = 0;

    uint32_t trailing_length = 0;

    uint32_t partial_sectors = 0;

    uint32_t bytes_written = 0;

    uint64_t file_offset;

    VHDXHeader *header;

    VHDXLogEntryHeader new_hdr;

    VHDXLogDescriptor *new_desc = NULL;

    VHDXLogDataSector *data_sector = NULL;

    MSGUID new_guid = { 0 };



    header = s->headers[s->curr_header];



    



    if (length > header->log_length) {

        

        ret = -EINVAL;

        goto exit;

    }



    if (guid_eq(header->log_guid, zero_guid)) {

        vhdx_guid_generate(&new_guid);

        vhdx_update_headers(bs, s, false, &new_guid);

    } else {

        

        ret = -ENOTSUP;

        goto exit;

    }



    

    if (s->log.sequence == 0) {

        s->log.sequence = 1;

    }



    sector_offset = offset % VHDX_LOG_SECTOR_SIZE;

    file_offset = (offset / VHDX_LOG_SECTOR_SIZE) * VHDX_LOG_SECTOR_SIZE;



    aligned_length = length;



    

    if (sector_offset) {

        leading_length = (VHDX_LOG_SECTOR_SIZE - sector_offset);

        leading_length = leading_length > length ? length : leading_length;

        aligned_length -= leading_length;

        partial_sectors++;

    }



    sectors = aligned_length / VHDX_LOG_SECTOR_SIZE;

    trailing_length = aligned_length - (sectors * VHDX_LOG_SECTOR_SIZE);

    if (trailing_length) {

        partial_sectors++;

    }



    sectors += partial_sectors;



    



    new_hdr = (VHDXLogEntryHeader) {

                .signature           = VHDX_LOG_SIGNATURE,

                .tail                = s->log.tail,

                .sequence_number     = s->log.sequence,

                .descriptor_count    = sectors,

                .reserved            = 0,

                .flushed_file_offset = bdrv_getlength(bs->file->bs),

                .last_file_offset    = bdrv_getlength(bs->file->bs),

              };



    new_hdr.log_guid = header->log_guid;



    desc_sectors = vhdx_compute_desc_sectors(new_hdr.descriptor_count);



    total_length = (desc_sectors + sectors) * VHDX_LOG_SECTOR_SIZE;

    new_hdr.entry_length = total_length;



    vhdx_log_entry_hdr_le_export(&new_hdr);



    buffer = qemu_blockalign(bs, total_length);

    memcpy(buffer, &new_hdr, sizeof(new_hdr));



    new_desc = buffer + sizeof(new_hdr);

    data_sector = buffer + (desc_sectors * VHDX_LOG_SECTOR_SIZE);

    data_tmp = data;



    

    merged_sector = qemu_blockalign(bs, VHDX_LOG_SECTOR_SIZE);



    for (i = 0; i < sectors; i++) {

        new_desc->signature       = VHDX_LOG_DESC_SIGNATURE;

        new_desc->sequence_number = s->log.sequence;

        new_desc->file_offset     = file_offset;



        if (i == 0 && leading_length) {

            

            ret = bdrv_pread(bs->file, file_offset, merged_sector,

                             VHDX_LOG_SECTOR_SIZE);

            if (ret < 0) {

                goto exit;

            }

            memcpy(merged_sector + sector_offset, data_tmp, leading_length);

            bytes_written = leading_length;

            sector_write = merged_sector;

        } else if (i == sectors - 1 && trailing_length) {

            

            ret = bdrv_pread(bs->file,

                            file_offset,

                            merged_sector + trailing_length,

                            VHDX_LOG_SECTOR_SIZE - trailing_length);

            if (ret < 0) {

                goto exit;

            }

            memcpy(merged_sector, data_tmp, trailing_length);

            bytes_written = trailing_length;

            sector_write = merged_sector;

        } else {

            bytes_written = VHDX_LOG_SECTOR_SIZE;

            sector_write = data_tmp;

        }



        

        vhdx_log_raw_to_le_sector(new_desc, data_sector, sector_write,

                                  s->log.sequence);



        data_tmp += bytes_written;

        data_sector++;

        new_desc++;

        file_offset += VHDX_LOG_SECTOR_SIZE;

    }



    

    vhdx_update_checksum(buffer, total_length,

                         offsetof(VHDXLogEntryHeader, checksum));



    

    ret = vhdx_log_write_sectors(bs, &s->log, &sectors_written, buffer,

                                 desc_sectors + sectors);

    if (ret < 0) {

        goto exit;

    }



    if (sectors_written != desc_sectors + sectors) {

        

        ret = -EINVAL;

        goto exit;

    }



    s->log.sequence++;

    

    s->log.tail = s->log.write;



exit:

    qemu_vfree(buffer);

    qemu_vfree(merged_sector);

    return ret;

}