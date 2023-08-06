int avpriv_vsnprintf(char *restrict s, size_t n, const char *restrict fmt,

                     va_list ap)

{

    int ret;



    if (n == 0)

        return 0;

    else if (n > INT_MAX)

        return AVERROR(EINVAL);



    

    memset(s, 0, n);

    ret = vsnprintf(s, n - 1, fmt, ap);

    if (ret == -1)

        ret = n;



    return ret;

}