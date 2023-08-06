static int exec_close(MigrationState *s)

{

    int ret = 0;

    DPRINTF("exec_close\n");

    ret = qemu_fclose(s->opaque);

    s->opaque = NULL;

    s->fd = -1;

    if (ret >= 0 && !(WIFEXITED(ret) && WEXITSTATUS(ret) == 0)) {

        

        ret = -EIO; 

    }

    return ret;

}