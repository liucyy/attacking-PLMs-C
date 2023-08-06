void *av_malloc(unsigned int size)

{

    void *ptr;

    

#if defined (HAVE_MEMALIGN)

    ptr = memalign(16,size);

    

     

     

#else

    ptr = malloc(size);

#endif

    return ptr;

}