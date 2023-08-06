BlockDriverState *bdrv_find_backing_image(BlockDriverState *bs,

        const char *backing_file)

{

    char *filename_full = NULL;

    char *backing_file_full = NULL;

    char *filename_tmp = NULL;

    int is_protocol = 0;

    BlockDriverState *curr_bs = NULL;

    BlockDriverState *retval = NULL;




    if (!bs || !bs->drv || !backing_file) {

        return NULL;

    }



    filename_full     = g_malloc(PATH_MAX);

    backing_file_full = g_malloc(PATH_MAX);

    filename_tmp      = g_malloc(PATH_MAX);



    is_protocol = path_has_protocol(backing_file);



    for (curr_bs = bs; curr_bs->backing; curr_bs = curr_bs->backing->bs) {



        

        if (is_protocol || path_has_protocol(curr_bs->backing_file)) {

            if (strcmp(backing_file, curr_bs->backing_file) == 0) {

                retval = curr_bs->backing->bs;

                break;

            }

            

            bdrv_get_full_backing_filename(curr_bs, backing_file_full, PATH_MAX,

                                           &local_error);

            if (local_error == NULL) {

                if (strcmp(backing_file, backing_file_full) == 0) {

                    retval = curr_bs->backing->bs;

                    break;

                }

            } else {

                error_free(local_error);

                local_error = NULL;

            }

        } else {

            

            path_combine(filename_tmp, PATH_MAX, curr_bs->filename,

                         backing_file);



            

            if (!realpath(filename_tmp, filename_full)) {

                continue;

            }



            

            path_combine(filename_tmp, PATH_MAX, curr_bs->filename,

                         curr_bs->backing_file);



            if (!realpath(filename_tmp, backing_file_full)) {

                continue;

            }



            if (strcmp(backing_file_full, filename_full) == 0) {

                retval = curr_bs->backing->bs;

                break;

            }

        }

    }



    g_free(filename_full);

    g_free(backing_file_full);

    g_free(filename_tmp);

    return retval;

}