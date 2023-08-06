void *av_malloc(size_t size)

{

    void *ptr = NULL;

#if CONFIG_MEMALIGN_HACK

    long diff;

#endif



    

    if(size > (INT_MAX-32) )

        return NULL;



#if CONFIG_MEMALIGN_HACK

    ptr = malloc(size+32);

    if(!ptr)

        return ptr;

    diff= ((-(long)ptr - 1)&31) + 1;

    ptr = (char*)ptr + diff;

    ((char*)ptr)[-1]= diff;

#elif HAVE_POSIX_MEMALIGN

    if (posix_memalign(&ptr,32,size))

        ptr = NULL;

#elif HAVE_MEMALIGN

    ptr = memalign(32,size);

    

     

     

#else

    ptr = malloc(size);

#endif

    return ptr;

}