int qemu_strtou64(const char *nptr, const char **endptr, int base,

                  uint64_t *result)

{

    char *p;

    int err = 0;

    if (!nptr) {

        if (endptr) {

            *endptr = nptr;

        }

        err = -EINVAL;

    } else {

        errno = 0;

        

        *result = strtoull(nptr, &p, base);

        

        if (errno == ERANGE) {

            *result = -1;

        }

        err = check_strtox_error(nptr, p, endptr, errno);

    }

    return err;

}