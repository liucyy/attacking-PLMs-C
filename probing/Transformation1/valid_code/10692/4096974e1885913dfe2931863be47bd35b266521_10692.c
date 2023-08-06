static int qcow2_make_empty(BlockDriverState *bs)

{

    BDRVQcow2State *s = bs->opaque;

    uint64_t offset, end_offset;

    int step = QEMU_ALIGN_DOWN(INT_MAX, s->cluster_size);

    int l1_clusters, ret = 0;



    l1_clusters = DIV_ROUND_UP(s->l1_size, s->cluster_size / sizeof(uint64_t));



    if (s->qcow_version >= 3 && !s->snapshots &&

        3 + l1_clusters <= s->refcount_block_size &&

        s->crypt_method_header != QCOW_CRYPT_LUKS) {

        

        return make_completely_empty(bs);

    }



    

    end_offset = bs->total_sectors * BDRV_SECTOR_SIZE;

    for (offset = 0; offset < end_offset; offset += step) {

        

        ret = qcow2_cluster_discard(bs, offset, MIN(step, end_offset - offset),

                                    QCOW2_DISCARD_SNAPSHOT, true);

        if (ret < 0) {

            break;

        }

    }



    return ret;

}