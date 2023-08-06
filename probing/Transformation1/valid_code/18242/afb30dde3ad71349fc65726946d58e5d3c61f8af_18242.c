uri_resolve_relative (const char *uri, const char * base)

{

    char *val = NULL;

    int ret;

    int ix;

    int pos = 0;

    int nbslash = 0;

    int len;

    URI *ref = NULL;

    URI *bas = NULL;

    char *bptr, *uptr, *vptr;

    int remove_path = 0;



    if ((uri == NULL) || (*uri == 0))

	return NULL;



    

    ref = uri_new ();

    

    if (uri[0] != '.') {

	ret = uri_parse_into (ref, uri);

	if (ret != 0)

	    goto done;		

    } else

	ref->path = g_strdup(uri);



    

    if ((base == NULL) || (*base == 0)) {

	val = g_strdup (uri);

	goto done;

    }

    bas = uri_new ();

    if (base[0] != '.') {

	ret = uri_parse_into (bas, base);

	if (ret != 0)

	    goto done;		

    } else

	bas->path = g_strdup(base);



    

    if ((ref->scheme != NULL) &&

	((bas->scheme == NULL) ||

	 (strcmp (bas->scheme, ref->scheme)) ||

	 (strcmp (bas->server, ref->server)))) {

	val = g_strdup (uri);

	goto done;

    }

    if (!strcmp(bas->path, ref->path)) {

	val = g_strdup("");

	goto done;

    }

    if (bas->path == NULL) {

	val = g_strdup(ref->path);

	goto done;

    }

    if (ref->path == NULL) {

        ref->path = (char *) "/";

	remove_path = 1;

    }



    

    if (bas->path == NULL) {

	if (ref->path != NULL) {

	    uptr = ref->path;

	    if (*uptr == '/')

		uptr++;

	    

	    val = uri_string_escape(uptr, "/;&=+$,");

	}

	goto done;

    }

    bptr = bas->path;

    if (ref->path == NULL) {

	for (ix = 0; bptr[ix] != 0; ix++) {

	    if (bptr[ix] == '/')

		nbslash++;

	}

	uptr = NULL;

	len = 1;	

    } else {

    

	if ((ref->path[pos] == '.') && (ref->path[pos+1] == '/'))

            pos += 2;

	if ((*bptr == '.') && (bptr[1] == '/'))

            bptr += 2;

	else if ((*bptr == '/') && (ref->path[pos] != '/'))

	    bptr++;

	while ((bptr[pos] == ref->path[pos]) && (bptr[pos] != 0))

	    pos++;



	if (bptr[pos] == ref->path[pos]) {

	    val = g_strdup("");

	    goto done;		

	}



	

	ix = pos;

	if ((ref->path[ix] == '/') && (ix > 0))

	    ix--;

	else if ((ref->path[ix] == 0) && (ix > 1) && (ref->path[ix - 1] == '/'))

	    ix -= 2;

	for (; ix > 0; ix--) {

	    if (ref->path[ix] == '/')

		break;

	}

	if (ix == 0) {

	    uptr = ref->path;

	} else {

	    ix++;

	    uptr = &ref->path[ix];

	}



	

	if (bptr[pos] != ref->path[pos]) {

	    for (; bptr[ix] != 0; ix++) {

		if (bptr[ix] == '/')

		    nbslash++;

	    }

	}

	len = strlen (uptr) + 1;

    }



    if (nbslash == 0) {

	if (uptr != NULL)

	    

	    val = uri_string_escape(uptr, "/;&=+$,");

	goto done;

    }



    

    val = g_malloc (len + 3 * nbslash);

    vptr = val;

    

    for (; nbslash>0; nbslash--) {

	*vptr++ = '.';

	*vptr++ = '.';

	*vptr++ = '/';

    }

    

    if (uptr != NULL) {

        if ((vptr > val) && (len > 0) &&

	    (uptr[0] == '/') && (vptr[-1] == '/')) {

	    memcpy (vptr, uptr + 1, len - 1);

	    vptr[len - 2] = 0;

	} else {

	    memcpy (vptr, uptr, len);

	    vptr[len - 1] = 0;

	}

    } else {

	vptr[len - 1] = 0;

    }



    

    vptr = val;

	

    val = uri_string_escape(vptr, "/;&=+$,");

    g_free(vptr);



done:

    

    if (remove_path != 0)

        ref->path = NULL;

    if (ref != NULL)

	uri_free (ref);

    if (bas != NULL)

	uri_free (bas);



    return val;

}