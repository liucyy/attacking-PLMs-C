static int get_whole_cluster(BlockDriverState *bs,

                VmdkExtent *extent,

                uint64_t cluster_offset,

                uint64_t offset,

                bool allocate)

{

    

    uint8_t  whole_grain[extent->cluster_sectors * 512];



    

    if (bs->backing_hd) {

        int ret;



        if (!vmdk_is_cid_valid(bs))

            return -1;



        

        offset -= offset % (extent->cluster_sectors * 512);

        ret = bdrv_read(bs->backing_hd, offset >> 9, whole_grain,

                extent->cluster_sectors);

        if (ret < 0) {

            return -1;

        }



        

        ret = bdrv_write(extent->file, cluster_offset, whole_grain,

                extent->cluster_sectors);

        if (ret < 0) {

            return -1;

        }

    }

    return 0;

}