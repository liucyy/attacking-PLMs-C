void bdrv_refresh_filename(BlockDriverState *bs)

{

    BlockDriver *drv = bs->drv;

    QDict *opts;



    if (!drv) {

        return;

    }



    

    if (bs->file) {

        bdrv_refresh_filename(bs->file->bs);

    }



    if (drv->bdrv_refresh_filename) {

        

        bs->exact_filename[0] = '\0';

        if (bs->full_open_options) {

            QDECREF(bs->full_open_options);

            bs->full_open_options = NULL;

        }



        drv->bdrv_refresh_filename(bs);

    } else if (bs->file) {

        

        bool has_open_options;



        bs->exact_filename[0] = '\0';

        if (bs->full_open_options) {

            QDECREF(bs->full_open_options);

            bs->full_open_options = NULL;

        }



        opts = qdict_new();

        has_open_options = append_open_options(opts, bs);



        

        if (bs->file->bs->exact_filename[0] && !has_open_options) {

            strcpy(bs->exact_filename, bs->file->bs->exact_filename);

        }

        

        if (bs->file->bs->full_open_options) {

            qdict_put_obj(opts, "driver",

                          QOBJECT(qstring_from_str(drv->format_name)));

            QINCREF(bs->file->bs->full_open_options);

            qdict_put_obj(opts, "file",

                          QOBJECT(bs->file->bs->full_open_options));



            bs->full_open_options = opts;

        } else {

            QDECREF(opts);

        }

    } else if (!bs->full_open_options && qdict_size(bs->options)) {

        



        opts = qdict_new();

        append_open_options(opts, bs);

        qdict_put_obj(opts, "driver",

                      QOBJECT(qstring_from_str(drv->format_name)));



        if (bs->exact_filename[0]) {

            

            qdict_put_obj(opts, "filename",

                          QOBJECT(qstring_from_str(bs->exact_filename)));

        }



        bs->full_open_options = opts;

    }



    if (bs->exact_filename[0]) {

        pstrcpy(bs->filename, sizeof(bs->filename), bs->exact_filename);

    } else if (bs->full_open_options) {

        QString *json = qobject_to_json(QOBJECT(bs->full_open_options));

        snprintf(bs->filename, sizeof(bs->filename), "json:%s",

                 qstring_get_str(json));

        QDECREF(json);

    }

}