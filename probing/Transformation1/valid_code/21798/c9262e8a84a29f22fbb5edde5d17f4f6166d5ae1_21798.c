void zipl_load(void)

{

    ScsiMbr *mbr = (void *)sec;

    LDL_VTOC *vlbl = (void *)sec;



    

    memset(sec, FREE_SPACE_FILLER, sizeof(sec));

    read_block(0, mbr, "Cannot read block 0");



    dputs("checking magic\n");



    if (magic_match(mbr->magic, ZIPL_MAGIC)) {

        ipl_scsi(); 

    }



    

    if (virtio_guessed_disk_nature()) {

        virtio_assume_iso9660();

    }

    ipl_iso_el_torito();



    

    if (virtio_guessed_disk_nature()) {

        sclp_print("Using guessed DASD geometry.\n");

        virtio_assume_eckd();

    }

    print_eckd_msg();

    if (magic_match(mbr->magic, IPL1_MAGIC)) {

        ipl_eckd_cdl(); 

    }



    

    memset(sec, FREE_SPACE_FILLER, sizeof(sec));

    read_block(2, vlbl, "Cannot read block 2");



    if (magic_match(vlbl->magic, CMS1_MAGIC)) {

        ipl_eckd_ldl(ECKD_CMS); 

    }

    if (magic_match(vlbl->magic, LNX1_MAGIC)) {

        ipl_eckd_ldl(ECKD_LDL); 

    }



    ipl_eckd_ldl(ECKD_LDL_UNLABELED); 

    

    ipl_eckd_cdl();



    virtio_panic("\n* this can never happen *\n");

}