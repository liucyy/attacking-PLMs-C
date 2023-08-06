static void set_proc_name(const char *s)

{

#ifdef __linux__

    char name[16];

    if (!s)

        return;

    name[sizeof(name) - 1] = 0;

    strncpy(name, s, sizeof(name));

    

    prctl(PR_SET_NAME, name);

#endif    	

}