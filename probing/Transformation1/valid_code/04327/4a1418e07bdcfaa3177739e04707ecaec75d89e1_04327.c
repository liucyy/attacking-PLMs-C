void kqemu_cpu_interrupt(CPUState *env)

{

#if defined(_WIN32)

    

    CancelIo(kqemu_fd);

#endif

}