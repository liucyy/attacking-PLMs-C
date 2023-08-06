void cpu_set_log(int log_flags)

{

    loglevel = log_flags;

    if (loglevel && !logfile) {

        logfile = fopen(logfilename, log_append ? "a" : "w");

        if (!logfile) {

            perror(logfilename);

            _exit(1);

        }

#if !defined(CONFIG_SOFTMMU)

        

        {

            static char logfile_buf[4096];

            setvbuf(logfile, logfile_buf, _IOLBF, sizeof(logfile_buf));

        }

#elif !defined(_WIN32)

        

        setvbuf(logfile, NULL, _IOLBF, 0);

#endif

        log_append = 1;

    }

    if (!loglevel && logfile) {

        fclose(logfile);

        logfile = NULL;

    }

}