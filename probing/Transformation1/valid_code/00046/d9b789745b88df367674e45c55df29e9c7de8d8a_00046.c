static int find_allocation(BlockDriverState *bs, off_t start,

                           off_t *data, off_t *hole)

{

    BDRVGlusterState *s = bs->opaque;

    off_t offs;



    if (!s->supports_seek_data) {

        return -ENOTSUP;

    }



    

    offs = glfs_lseek(s->fd, start, SEEK_DATA);

    if (offs < 0) {

        return -errno;          

    }

    assert(offs >= start);



    if (offs > start) {

        

        *hole = start;

        *data = offs;

        return 0;

    }



    



    

    offs = glfs_lseek(s->fd, start, SEEK_HOLE);

    if (offs < 0) {

        return -errno;          

    }

    assert(offs >= start);



    if (offs > start) {

        

        *data = start;

        *hole = offs;

        return 0;

    }



    

    return -EBUSY;

}