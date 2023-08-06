static int raw_fd_pool_get(BDRVRawState *s)

{

    int i;



    for (i = 0; i < RAW_FD_POOL_SIZE; i++) {

        

        if (s->fd_pool[i] != -1)

            continue;



        

        s->fd_pool[i] = dup(s->fd);

        if (s->fd_pool[i] != -1)

            return s->fd_pool[i];

    }



    

    return s->fd;

}