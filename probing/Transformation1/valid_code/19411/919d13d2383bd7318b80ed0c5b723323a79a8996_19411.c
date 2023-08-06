static int cookie_string(AVDictionary *dict, char **cookies)

{

    AVDictionaryEntry *e = NULL;

    int len = 1;



    

    while (e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX))

        len += strlen(e->key) + strlen(e->value) + 1;



    

    e = NULL;

    if (*cookies) av_free(*cookies);

    *cookies = av_malloc(len);

    if (!cookies) return AVERROR(ENOMEM);

    *cookies[0] = '\0';



    

    while (e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX))

        av_strlcatf(*cookies, len, "%s%s\n", e->key, e->value);



    return 0;

}