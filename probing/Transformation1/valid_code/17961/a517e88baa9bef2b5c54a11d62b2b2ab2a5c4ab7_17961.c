static void show_parts(const char *device)

{

    if (fork() == 0) {

        int nbd;



        

        nbd = open(device, O_RDWR);

        if (nbd != -1)

              close(nbd);

        exit(0);

    }

}