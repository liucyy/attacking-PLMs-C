static inline bool check_lba_range(SCSIDiskState *s,

                                   uint64_t sector_num, uint32_t nb_sectors)

{

    

    return (sector_num <= sector_num + nb_sectors &&

            sector_num + nb_sectors - 1 <= s->qdev.max_lba);

}