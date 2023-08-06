void hd_geometry_guess(BlockDriverState *bs,

                       int *pcyls, int *pheads, int *psecs)

{

    int cylinders, heads, secs, translation;



    bdrv_get_geometry_hint(bs, &cylinders, &heads, &secs);

    translation = bdrv_get_translation_hint(bs);



    if (cylinders != 0) {

        

        *pcyls = cylinders;

        *pheads = heads;

        *psecs = secs;

        return;

    }



    if (guess_disk_lchs(bs, &cylinders, &heads, &secs) < 0) {

        

        guess_chs_for_size(bs, pcyls, pheads, psecs);

    } else if (heads > 16) {

        

        guess_chs_for_size(bs, pcyls, pheads, psecs);

        if (translation == BIOS_ATA_TRANSLATION_AUTO) {

            bdrv_set_translation_hint(bs,

                                      *pcyls * *pheads <= 131072

                                      ? BIOS_ATA_TRANSLATION_LARGE

                                      : BIOS_ATA_TRANSLATION_LBA);

        }

    } else {

        

        *pcyls = cylinders;

        *pheads = heads;

        *psecs = secs;

        

        if (translation == BIOS_ATA_TRANSLATION_AUTO) {

            bdrv_set_translation_hint(bs,

                                      BIOS_ATA_TRANSLATION_NONE);

        }

    }

    bdrv_set_geometry_hint(bs, *pcyls, *pheads, *psecs);

    trace_hd_geometry_guess(bs, *pcyls, *pheads, *psecs, translation);

}