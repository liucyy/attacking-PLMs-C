DriveInfo *drive_get(BlockInterfaceType type, int bus, int unit)

{

    DriveInfo *dinfo;



    



    TAILQ_FOREACH(dinfo, &drives, next) {

        if (dinfo->type == type &&

	    dinfo->bus == bus &&

	    dinfo->unit == unit)

            return dinfo;

    }



    return NULL;

}