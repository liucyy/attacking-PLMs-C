real_parse_asm_rulebook(AVFormatContext *s, AVStream *orig_st,

                        const char *p)

{

    const char *end;

    int n_rules, odd = 0;

    AVStream *st;



    

    if (*p == '\"') p++;

    for (n_rules = 0; s->nb_streams < MAX_STREAMS;) {

        if (!(end = strchr(p, ';')))

            break;

        if (!odd && end != p) {

            if (n_rules > 0)

                st = add_dstream(s, orig_st);

            else

                st = orig_st;

            real_parse_asm_rule(st, p, end);

            n_rules++;

        }

        p = end + 1;

        odd ^= 1;

    }

}