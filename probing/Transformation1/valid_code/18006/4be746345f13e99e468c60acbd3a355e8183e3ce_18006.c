void hd_geometry_guess(BlockDriverState *bs,

                       uint32_t *pcyls, uint32_t *pheads, uint32_t *psecs,

                       int *ptrans)

{

    int cylinders, heads, secs, translation;



    if (guess_disk_lchs(bs, &cylinders, &heads, &secs) < 0) {

        

        guess_chs_for_size(bs, pcyls, pheads, psecs);

        translation = hd_bios_chs_auto_trans(*pcyls, *pheads, *psecs);

    } else if (heads > 16) {

        

        guess_chs_for_size(bs, pcyls, pheads, psecs);

        translation = *pcyls * *pheads <= 131072

            ? BIOS_ATA_TRANSLATION_LARGE

            : BIOS_ATA_TRANSLATION_LBA;

    } else {

        

        *pcyls = cylinders;

        *pheads = heads;

        *psecs = secs;

        

        translation = BIOS_ATA_TRANSLATION_NONE;

    }

    if (ptrans) {

        *ptrans = translation;

    }

    trace_hd_geometry_guess(bs, *pcyls, *pheads, *psecs, translation);

}