VirtIODevice *virtio_9p_init(DeviceState *dev, V9fsConf *conf)

 {

    V9fsState *s;

    int i, len;

    struct stat stat;

    FsTypeEntry *fse;





    s = (V9fsState *)virtio_common_init("virtio-9p",

                                    VIRTIO_ID_9P,

                                    sizeof(struct virtio_9p_config)+

                                    MAX_TAG_LEN,

                                    sizeof(V9fsState));



    

    QLIST_INIT(&s->free_list);

    for (i = 0; i < (MAX_REQ - 1); i++) {

	QLIST_INSERT_HEAD(&s->free_list, &s->pdus[i], next);

    }



    s->vq = virtio_add_queue(&s->vdev, MAX_REQ, handle_9p_output);



    fse = get_fsdev_fsentry(conf->fsdev_id);



    if (!fse) {

        

        fprintf(stderr, "Virtio-9p device couldn't find fsdev "

                    "with the id %s\n", conf->fsdev_id);

        exit(1);

    }



    if (!fse->path || !conf->tag) {

        

        fprintf(stderr, "fsdev with id %s needs path "

                "and Virtio-9p device needs mount_tag arguments\n",

                conf->fsdev_id);

        exit(1);

    }



    if (!strcmp(fse->security_model, "passthrough")) {

        

        s->ctx.fs_sm = SM_PASSTHROUGH;

    } else if (!strcmp(fse->security_model, "mapped")) {

        

        s->ctx.fs_sm = SM_MAPPED;

    } else {

        

        fprintf(stderr, "one of the following must be specified as the"

                "security option:\n\t security_model=passthrough \n\t "

                "security_model=mapped\n");

        return NULL;

    }



    if (lstat(fse->path, &stat)) {

        fprintf(stderr, "share path %s does not exist\n", fse->path);

        exit(1);

    } else if (!S_ISDIR(stat.st_mode)) {

        fprintf(stderr, "share path %s is not a directory \n", fse->path);

        exit(1);

    }



    s->ctx.fs_root = qemu_strdup(fse->path);

    len = strlen(conf->tag);

    if (len > MAX_TAG_LEN) {

        len = MAX_TAG_LEN;

    }

    

    s->tag = qemu_malloc(len);

    memcpy(s->tag, conf->tag, len);

    s->tag_len = len;

    s->ctx.uid = -1;



    s->ops = fse->ops;

    s->vdev.get_features = virtio_9p_get_features;

    s->config_size = sizeof(struct virtio_9p_config) +

                        s->tag_len;

    s->vdev.get_config = virtio_9p_get_config;



    return &s->vdev;

}