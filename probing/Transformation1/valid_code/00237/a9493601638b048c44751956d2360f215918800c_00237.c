void *av_realloc(void *ptr, unsigned int size)

{

#ifdef MEMALIGN_HACK

    int diff;

#endif



    

    if(size > INT_MAX)

        return NULL;



#ifdef MEMALIGN_HACK

    

    if(!ptr) return av_malloc(size);

    diff= ((char*)ptr)[-1];

    return realloc(ptr - diff, size + diff) + diff;

#else

    return realloc(ptr, size);

#endif

}