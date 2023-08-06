static int vpc_create(const char *filename, int64_t total_sectors,

    const char *backing_file, int flags)

{

    uint8_t buf[1024];

    struct vhd_footer* footer = (struct vhd_footer*) buf;

    struct vhd_dyndisk_header* dyndisk_header =

        (struct vhd_dyndisk_header*) buf;

    int fd, i;

    uint16_t cyls;

    uint8_t heads;

    uint8_t secs_per_cyl;

    size_t block_size, num_bat_entries;



    if (backing_file != NULL)

        return -ENOTSUP;



    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);

    if (fd < 0)

        return -EIO;



    

    calculate_geometry(total_sectors, &cyls, &heads, &secs_per_cyl);

    total_sectors = (int64_t) cyls * heads * secs_per_cyl;



    

    memset(buf, 0, 1024);



    strncpy(footer->creator, "conectix", 8);

    

    strncpy(footer->creator_app, "qemu", 4);

    strncpy(footer->creator_os, "Wi2k", 4);



    footer->features = be32_to_cpu(0x02);

    footer->version = be32_to_cpu(0x00010000);

    footer->data_offset = be64_to_cpu(HEADER_SIZE);

    footer->timestamp = be32_to_cpu(time(NULL) - VHD_TIMESTAMP_BASE);



    

    footer->major = be16_to_cpu(0x0005);

    footer->minor =be16_to_cpu(0x0003);



    footer->orig_size = be64_to_cpu(total_sectors * 512);

    footer->size = be64_to_cpu(total_sectors * 512);



    footer->cyls = be16_to_cpu(cyls);

    footer->heads = heads;

    footer->secs_per_cyl = secs_per_cyl;



    footer->type = be32_to_cpu(VHD_DYNAMIC);



    



    footer->checksum = be32_to_cpu(vpc_checksum(buf, HEADER_SIZE));



    

    block_size = 0x200000;

    num_bat_entries = (total_sectors + block_size / 512) / (block_size / 512);



    if (write(fd, buf, HEADER_SIZE) != HEADER_SIZE)

        return -EIO;



    if (lseek(fd, 1536 + ((num_bat_entries * 4 + 511) & ~511), SEEK_SET) < 0)

        return -EIO;

    if (write(fd, buf, HEADER_SIZE) != HEADER_SIZE)

        return -EIO;



    

    if (lseek(fd, 3 * 512, SEEK_SET) < 0)

        return -EIO;



    memset(buf, 0xFF, 512);

    for (i = 0; i < (num_bat_entries * 4 + 511) / 512; i++)

        if (write(fd, buf, 512) != 512)

            return -EIO;





    

    memset(buf, 0, 1024);



    strncpy(dyndisk_header->magic, "cxsparse", 8);



    dyndisk_header->data_offset = be64_to_cpu(0xFFFFFFFF);

    dyndisk_header->table_offset = be64_to_cpu(3 * 512);

    dyndisk_header->version = be32_to_cpu(0x00010000);

    dyndisk_header->block_size = be32_to_cpu(block_size);

    dyndisk_header->max_table_entries = be32_to_cpu(num_bat_entries);



    dyndisk_header->checksum = be32_to_cpu(vpc_checksum(buf, 1024));



    

    if (lseek(fd, 512, SEEK_SET) < 0)

        return -EIO;

    if (write(fd, buf, 1024) != 1024)

        return -EIO;



    close(fd);

    return 0;

}