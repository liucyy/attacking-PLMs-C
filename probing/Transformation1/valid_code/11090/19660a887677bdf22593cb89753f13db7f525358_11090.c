void av_url_split(char *proto, int proto_size,

                  char *authorization, int authorization_size,

                  char *hostname, int hostname_size,

                  int *port_ptr,

                  char *path, int path_size,

                  const char *url)

{

    const char *p, *ls, *ls2, *at, *col, *brk;



    if (port_ptr)               *port_ptr = -1;

    if (proto_size > 0)         proto[0] = 0;

    if (authorization_size > 0) authorization[0] = 0;

    if (hostname_size > 0)      hostname[0] = 0;

    if (path_size > 0)          path[0] = 0;



    

    if ((p = strchr(url, ':'))) {

        av_strlcpy(proto, url, FFMIN(proto_size, p + 1 - url));

        p++; 

        if (*p == '/') p++;

        if (*p == '/') p++;

    } else {

        

        av_strlcpy(path, url, path_size);

        return;

    }



    

    ls = strchr(p, '/');

    ls2 = strchr(p, '?');

    if(!ls)

        ls = ls2;

    else if (ls && ls2)

        ls = FFMIN(ls, ls2);

    if(ls)

        av_strlcpy(path, ls, path_size);

    else

        ls = &p[strlen(p)]; 



    

    if (ls != p) {

        

        if ((at = strchr(p, '@')) && at < ls) {

            av_strlcpy(authorization, p,

                       FFMIN(authorization_size, at + 1 - p));

            p = at + 1; 

        }



        if (*p == '[' && (brk = strchr(p, ']')) && brk < ls) {

            

            av_strlcpy(hostname, p + 1,

                       FFMIN(hostname_size, brk - p));

            if (brk[1] == ':' && port_ptr)

                *port_ptr = atoi(brk + 2);

        } else if ((col = strchr(p, ':')) && col < ls) {

            av_strlcpy(hostname, p,

                       FFMIN(col + 1 - p, hostname_size));

            if (port_ptr) *port_ptr = atoi(col + 1);

        } else

            av_strlcpy(hostname, p,

                       FFMIN(ls + 1 - p, hostname_size));

    }

}