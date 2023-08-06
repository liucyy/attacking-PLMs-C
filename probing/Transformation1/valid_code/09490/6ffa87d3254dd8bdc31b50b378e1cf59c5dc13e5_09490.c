int av_tempfile(char *prefix, char **filename) {

    int fd=-1;

#ifdef __MINGW32__

    *filename = tempnam(".", prefix);

#else

    size_t len = strlen(prefix) + 12; 

    *filename = av_malloc(len);

#endif

    

    if (*filename == NULL) {

        av_log(NULL, AV_LOG_ERROR, "ff_tempfile: Cannot allocate file name\n");

        return -1;

    }

#ifdef __MINGW32__

    fd = open(*filename, _O_RDWR | _O_BINARY | _O_CREAT, 0444);

#else

    snprintf(*filename, len, "/tmp/%sXXXXXX", prefix);

    fd = mkstemp(*filename);

    if (fd < 0) {

        snprintf(*filename, len, "./%sXXXXXX", prefix);

        fd = mkstemp(*filename);

    }

#endif

    

    if (fd < 0) {

        av_log(NULL, AV_LOG_ERROR, "ff_tempfile: Cannot open temporary file %s\n", *filename);

        return -1;

    }

    return fd; 

}