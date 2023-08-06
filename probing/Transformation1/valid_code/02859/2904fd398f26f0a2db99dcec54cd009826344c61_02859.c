int ff_mov_iso639_to_lang(const char *lang, int mp4)

{

    int i, code = 0;



    

    for (i = 0; !mp4 && i < FF_ARRAY_ELEMS(mov_mdhd_language_map); i++) {

        if (mov_mdhd_language_map[i] && !strcmp(lang, mov_mdhd_language_map[i]))

            return i;

    }

    

    if (!mp4)

        return 0;

    

    if (lang[0] == '\0')

        lang = "und";

    

    for (i = 0; i < 3; i++) {

        unsigned char c = (unsigned char)lang[i];

        if (c < 0x60)

            return 0;

        if (c > 0x60 + 0x1f)

            return 0;

        code <<= 5;

        code |= (c - 0x60);

    }

    return code;

}