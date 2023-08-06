static int bdrv_inherited_flags(int flags)

{

    

    flags |= BDRV_O_PROTOCOL;



    

    flags |= BDRV_O_CACHE_WB | BDRV_O_UNMAP;



    

    if (flags & BDRV_O_SNAPSHOT) {

        flags &= ~BDRV_O_RDWR;

    }



    

    flags &= ~(BDRV_O_SNAPSHOT | BDRV_O_NO_BACKING);



    return flags;

}