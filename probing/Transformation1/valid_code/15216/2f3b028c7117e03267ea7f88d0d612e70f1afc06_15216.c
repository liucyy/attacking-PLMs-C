static char *json_escape_str(const char *s)

{

    static const char json_escape[] = {'"', '\\', '\b', '\f', '\n', '\r', '\t', 0};

    static const char json_subst[]  = {'"', '\\',  'b',  'f',  'n',  'r',  't', 0};

    char *ret, *p;

    int i, len = 0;



    

    for (i = 0; s[i]; i++) {

        if (strchr(json_escape, s[i]))     len += 2; 

        else if ((unsigned char)s[i] < 32) len += 6; 

        else                               len += 1; 

    }



    p = ret = av_malloc(len + 1);

    if (!p)

        return NULL;

    for (i = 0; s[i]; i++) {

        char *q = strchr(json_escape, s[i]);

        if (q) {

            *p++ = '\\';

            *p++ = json_subst[q - json_escape];

        } else if ((unsigned char)s[i] < 32) {

            snprintf(p, 7, "\\u00%02x", s[i] & 0xff);

            p += 6;

        } else {

            *p++ = s[i];

        }

    }

    *p = 0;

    return ret;

}