void g_free(void *ptr)

{

    

    size_t *p;

    p = (size_t *)((char *)ptr - 16);

    munmap(p, *p);

}