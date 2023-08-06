static int blkverify_open(BlockDriverState *bs, const char *filename, int flags)

{

    BDRVBlkverifyState *s = bs->opaque;

    int ret;

    char *raw, *c;



    

    if (strncmp(filename, "blkverify:", strlen("blkverify:"))) {

        return -EINVAL;

    }

    filename += strlen("blkverify:");



    

    c = strchr(filename, ':');

    if (c == NULL) {

        return -EINVAL;

    }



    raw = strdup(filename);

    raw[c - filename] = '\0';

    ret = bdrv_file_open(&bs->file, raw, flags);

    free(raw);

    if (ret < 0) {

        return ret;

    }

    filename = c + 1;



    

    s->test_file = bdrv_new("");

    ret = bdrv_open(s->test_file, filename, flags, NULL);

    if (ret < 0) {

        bdrv_delete(s->test_file);

        s->test_file = NULL;

        return ret;

    }



    return 0;

}