static void bdrv_inherited_options(int *child_flags, QDict *child_options,

                                   int parent_flags, QDict *parent_options)

{

    int flags = parent_flags;



    

    flags |= BDRV_O_PROTOCOL;



    

    qdict_copy_default(child_options, parent_options, BDRV_OPT_CACHE_DIRECT);

    qdict_copy_default(child_options, parent_options, BDRV_OPT_CACHE_NO_FLUSH);



    

    qdict_copy_default(child_options, parent_options, BDRV_OPT_READ_ONLY);



    

    flags |= BDRV_O_UNMAP;



    

    flags &= ~(BDRV_O_SNAPSHOT | BDRV_O_NO_BACKING | BDRV_O_COPY_ON_READ |

               BDRV_O_NO_IO);



    *child_flags = flags;

}