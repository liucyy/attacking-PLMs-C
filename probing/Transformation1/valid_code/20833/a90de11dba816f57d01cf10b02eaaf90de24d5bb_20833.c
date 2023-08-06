void *av_malloc(unsigned int size)

{

    void *ptr = NULL;

#if CONFIG_MEMALIGN_HACK

    long diff;

#endif



    

    if(size > (INT_MAX-16) )

        return NULL;



#if CONFIG_MEMALIGN_HACK

    ptr = malloc(size+16);

    if(!ptr)

        return ptr;

    diff= ((-(long)ptr - 1)&15) + 1;

    ptr = (char*)ptr + diff;

    ((char*)ptr)[-1]= diff;

#elif HAVE_POSIX_MEMALIGN

    posix_memalign(&ptr,16,size);

#elif HAVE_MEMALIGN

    ptr = memalign(16,size);

    

     

     

#else

    ptr = malloc(size);

#endif

    return ptr;

}