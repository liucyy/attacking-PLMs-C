int av_parse_time(int64_t *timeval, const char *timestr, int duration)

{

    const char *p;

    int64_t t;

    struct tm dt = { 0 };

    int i;

    static const char * const date_fmt[] = {

        "%Y-%m-%d",

        "%Y%m%d",

    };

    static const char * const time_fmt[] = {

        "%H:%M:%S",

        "%H%M%S",

    };

    const char *q;

    int is_utc, len;

    char lastch;

    int negative = 0;



#undef time

    time_t now = time(0);



    len = strlen(timestr);

    if (len > 0)

        lastch = timestr[len - 1];

    else

        lastch = '\0';

    is_utc = (lastch == 'z' || lastch == 'Z');



    p = timestr;

    q = NULL;

    if (!duration) {

        if (!av_strncasecmp(timestr, "now", len)) {

            *timeval = (int64_t) now * 1000000;

            return 0;

        }



        

        for (i = 0; i < FF_ARRAY_ELEMS(date_fmt); i++) {

            q = small_strptime(p, date_fmt[i], &dt);

            if (q) {

                break;

            }

        }



        

        if (!q) {

            if (is_utc) {

                dt = *gmtime(&now);

            } else {

                dt = *localtime(&now);

            }

            dt.tm_hour = dt.tm_min = dt.tm_sec = 0;

        } else {

            p = q;

        }



        if (*p == 'T' || *p == 't' || *p == ' ')

            p++;



        

        for (i = 0; i < FF_ARRAY_ELEMS(time_fmt); i++) {

            q = small_strptime(p, time_fmt[i], &dt);

            if (q) {

                break;

            }

        }

    } else {

        

        if (p[0] == '-') {

            negative = 1;

            ++p;

        }

        

        q = small_strptime(p, time_fmt[0], &dt);

        if (!q) {

            

            dt.tm_sec = strtol(p, (void *)&q, 10);

            if (q == p) {

                

                *timeval = INT64_MIN;

                return AVERROR(EINVAL);

            }

            dt.tm_min = 0;

            dt.tm_hour = 0;

        }

    }



    

    if (!q) {

        *timeval = INT64_MIN;

        return AVERROR(EINVAL);

    }



    if (duration) {

        t = dt.tm_hour * 3600 + dt.tm_min * 60 + dt.tm_sec;

    } else {

        dt.tm_isdst = -1;       

        if (is_utc) {

            t = av_timegm(&dt);

        } else {

            t = mktime(&dt);

        }

    }



    t *= 1000000;



    

    if (*q == '.') {

        int val, n;

        q++;

        for (val = 0, n = 100000; n >= 1; n /= 10, q++) {

            if (!isdigit(*q))

                break;

            val += n * (*q - '0');

        }

        t += val;

    }

    *timeval = negative ? -t : t;

    return 0;

}