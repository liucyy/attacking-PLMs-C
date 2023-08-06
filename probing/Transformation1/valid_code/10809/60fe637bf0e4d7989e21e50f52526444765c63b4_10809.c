static int stdio_fclose(void *opaque)

{

    QEMUFileStdio *s = opaque;

    int ret = 0;



    if (qemu_file_is_writable(s->file)) {

        int fd = fileno(s->stdio_file);

        struct stat st;



        ret = fstat(fd, &st);

        if (ret == 0 && S_ISREG(st.st_mode)) {

            

            ret = fsync(fd);

            if (ret != 0) {

                ret = -errno;

                return ret;

            }

        }

    }

    if (fclose(s->stdio_file) == EOF) {

        ret = -errno;

    }

    g_free(s);

    return ret;

}