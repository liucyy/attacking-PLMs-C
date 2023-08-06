void trace_init_file(const char *file)

{

#ifdef CONFIG_TRACE_SIMPLE

    st_set_trace_file(file);

#elif defined CONFIG_TRACE_LOG

    

    if (file) {

        qemu_set_log_filename(file);

    }

#else

    if (file) {

        fprintf(stderr, "error: -trace file=...: "

                "option not supported by the selected tracing backends\n");

        exit(1);

    }

#endif

}