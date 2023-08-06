static int cmos_get_fd_drive_type(FloppyDriveType fd0)

{

    int val;



    switch (fd0) {

    case FLOPPY_DRIVE_TYPE_144:

        

        val = 4;

        break;

    case FLOPPY_DRIVE_TYPE_288:

        

        val = 5;

        break;

    case FLOPPY_DRIVE_TYPE_120:

        

        val = 2;

        break;

    case FLOPPY_DRIVE_TYPE_NONE:

    default:

        val = 0;

        break;

    }

    return val;

}