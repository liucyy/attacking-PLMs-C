static int open_self_cmdline(void *cpu_env, int fd)

{

    int fd_orig = -1;

    bool word_skipped = false;



    fd_orig = open("/proc/self/cmdline", O_RDONLY);

    if (fd_orig < 0) {

        return fd_orig;

    }



    while (true) {

        ssize_t nb_read;

        char buf[128];

        char *cp_buf = buf;



        nb_read = read(fd_orig, buf, sizeof(buf));

        if (nb_read < 0) {

            fd_orig = close(fd_orig);

            return -1;

        } else if (nb_read == 0) {

            break;

        }



        if (!word_skipped) {

            

            cp_buf = memchr(buf, 0, sizeof(buf));

            if (cp_buf) {

                

                cp_buf++;

                nb_read -= cp_buf - buf;

                word_skipped = true;

            }

        }



        if (word_skipped) {

            if (write(fd, cp_buf, nb_read) != nb_read) {


                return -1;

            }

        }

    }



    return close(fd_orig);

}