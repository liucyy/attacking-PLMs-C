static void quit_handler(int sig)
{
    
    if (ga_is_frozen(ga_state)) {
        return;
    g_debug("received signal num %d, quitting", sig);
    if (g_main_loop_is_running(ga_state->main_loop)) {
        g_main_loop_quit(ga_state->main_loop);