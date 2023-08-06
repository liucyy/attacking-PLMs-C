static int pick_geometry(FDrive *drv)

{

    BlockBackend *blk = drv->blk;

    const FDFormat *parse;

    uint64_t nb_sectors, size;

    int i;

    int match, size_match, type_match;

    bool magic = drv->drive == FLOPPY_DRIVE_TYPE_AUTO;



    

    if (!drv->blk || !blk_is_inserted(drv->blk) ||

        drv->drive == FLOPPY_DRIVE_TYPE_NONE)

    {

        return -1;

    }



    

    blk_get_geometry(blk, &nb_sectors);

    match = size_match = type_match = -1;

    for (i = 0; ; i++) {

        parse = &fd_formats[i];

        if (parse->drive == FLOPPY_DRIVE_TYPE_NONE) {

            break;

        }

        size = (parse->max_head + 1) * parse->max_track * parse->last_sect;

        if (nb_sectors == size) {

            if (magic || parse->drive == drv->drive) {

                

                goto out;

            } else if (drive_size(parse->drive) == drive_size(drv->drive)) {

                

                match = (match == -1) ? i : match;

            } else {

                

                size_match = (size_match == -1) ? i : size_match;

            }

        } else if (type_match == -1) {

            if ((parse->drive == drv->drive) ||

                (magic && (parse->drive == get_fallback_drive_type(drv)))) {

                

                type_match = i;

            }

        }

    }



    

    if (match == -1) {

        if (size_match != -1) {

            parse = &fd_formats[size_match];

            FLOPPY_DPRINTF("User requested floppy drive type '%s', "

                           "but inserted medium appears to be a "

                           "%d sector '%s' type\n",

                           FloppyDriveType_lookup[drv->drive],

                           nb_sectors,

                           FloppyDriveType_lookup[parse->drive]);

        }

        match = type_match;

    }



    

    if (match == -1) {

        error_setg(&error_abort, "No candidate geometries present in table "

                   " for floppy drive type '%s'",

                   FloppyDriveType_lookup[drv->drive]);

    }



    parse = &(fd_formats[match]);



 out:

    if (parse->max_head == 0) {

        drv->flags &= ~FDISK_DBL_SIDES;

    } else {

        drv->flags |= FDISK_DBL_SIDES;

    }

    drv->max_track = parse->max_track;

    drv->last_sect = parse->last_sect;

    drv->disk = parse->drive;

    drv->media_rate = parse->rate;

    return 0;

}