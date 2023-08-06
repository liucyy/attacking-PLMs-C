static int stdio_pclose(void *opaque)

{

    QEMUFileStdio *s = opaque;

    int ret;

    ret = pclose(s->stdio_file);

    if (ret == -1) {

        ret = -errno;

    } else if (!WIFEXITED(ret) || WEXITSTATUS(ret) != 0) {

        

        ret = -EIO; 

    }

    g_free(s);

    return ret;

}