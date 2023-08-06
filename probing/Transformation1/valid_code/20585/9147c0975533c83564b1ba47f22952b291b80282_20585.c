static int parse_cookie(HTTPContext *s, const char *p, AVDictionary **cookies)

{

    AVDictionary *new_params = NULL;

    AVDictionaryEntry *e, *cookie_entry;

    char *eql, *name;



    

    if (parse_set_cookie(p, &new_params))

        return -1;



    

    cookie_entry = av_dict_get(new_params, "", NULL, AV_DICT_IGNORE_SUFFIX);

    if (!cookie_entry || !cookie_entry->value) {


        return -1;

    }



    

    if ((e = av_dict_get(new_params, "expires", NULL, 0)) && e->value) {

        struct tm new_tm = {0};

        if (!parse_set_cookie_expiry_time(e->value, &new_tm)) {

            AVDictionaryEntry *e2;



            

            if (av_timegm(&new_tm) < av_gettime() / 1000000) {


                return -1;

            }



            

            e2 = av_dict_get(*cookies, cookie_entry->key, NULL, 0);

            if (e2 && e2->value) {

                AVDictionary *old_params = NULL;

                if (!parse_set_cookie(p, &old_params)) {

                    e2 = av_dict_get(old_params, "expires", NULL, 0);

                    if (e2 && e2->value) {

                        struct tm old_tm = {0};

                        if (!parse_set_cookie_expiry_time(e->value, &old_tm)) {

                            if (av_timegm(&new_tm) < av_timegm(&old_tm)) {


                                av_dict_free(&old_params);

                                return -1;

                            }

                        }

                    }

                }

                av_dict_free(&old_params);

            }

        }

    }




    

    if (!(eql = strchr(p, '='))) return AVERROR(EINVAL);

    if (!(name = av_strndup(p, eql - p))) return AVERROR(ENOMEM);



    

    av_dict_set(cookies, name, eql, AV_DICT_DONT_STRDUP_KEY);



    return 0;

}