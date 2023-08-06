static void init_mbr(BDRVVVFATState* s)

{

    

    mbr_t* real_mbr=(mbr_t*)s->first_sectors;

    partition_t* partition = &(real_mbr->partition[0]);

    int lba;



    memset(s->first_sectors,0,512);



    

    real_mbr->nt_id= cpu_to_le32(0xbe1afdfa);



    partition->attributes=0x80; 



    

    lba = sector2CHS(s->bs, &partition->start_CHS, s->first_sectors_number-1);

    lba|= sector2CHS(s->bs, &partition->end_CHS,   s->sector_count);



    

    partition->start_sector_long =cpu_to_le32(s->first_sectors_number-1);

    partition->length_sector_long=cpu_to_le32(s->sector_count - s->first_sectors_number+1);



    

    

    partition->fs_type= s->fat_type==12 ? 0x1:

                        s->fat_type==16 ? (lba?0xe:0x06):

                          (lba?0xc:0x0b);



    real_mbr->magic[0]=0x55; real_mbr->magic[1]=0xaa;

}