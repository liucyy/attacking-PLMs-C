static int parse_sandbox(void *opaque, QemuOpts *opts, Error **errp)
{
    if (qemu_opt_get_bool(opts, "enable", false)) {
#ifdef CONFIG_SECCOMP
        uint32_t seccomp_opts = QEMU_SECCOMP_SET_DEFAULT
                | QEMU_SECCOMP_SET_OBSOLETE;
        const char *value = NULL;
        value = qemu_opt_get(opts, "obsolete");
            if (g_str_equal(value, "allow")) {
                seccomp_opts &= ~QEMU_SECCOMP_SET_OBSOLETE;
            } else if (g_str_equal(value, "deny")) {
                
                error_report("invalid argument for obsolete");
        if (seccomp_start(seccomp_opts) < 0) {
            error_report("failed to install seccomp syscall filter "
                         "in the kernel");
#else
        error_report("seccomp support is disabled");
#endif
    return 0;