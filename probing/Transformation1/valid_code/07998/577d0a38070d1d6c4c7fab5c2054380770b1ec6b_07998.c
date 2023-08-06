void blkconf_serial(BlockConf *conf, char **serial)

{

    DriveInfo *dinfo;



    if (!*serial) {

        

        dinfo = drive_get_by_blockdev(conf->bs);

        if (*dinfo->serial) {

            *serial = g_strdup(dinfo->serial);

        }

    }

}