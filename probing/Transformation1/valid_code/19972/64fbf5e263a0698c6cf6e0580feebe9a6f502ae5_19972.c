static int parse_inputs(const char **buf, AVFilterInOut **currInputs,

                        AVFilterInOut **openLinks, AVClass *log_ctx)

{

    int pad = 0;



    while(**buf == '[') {

        char *name = parse_link_name(buf, log_ctx);

        AVFilterInOut *match;



        if(!name)

            return -1;



        

        match = extract_inout(name, openLinks);



        if(match) {

            

            if(match->type != LinkTypeOut) {

                av_log(log_ctx, AV_LOG_ERROR,

                       "Label \"%s\" appears twice as input!\n", match->name);

                return -1;

            }


        } else {

            

            match = av_mallocz(sizeof(AVFilterInOut));

            match->name    = name;

            match->type    = LinkTypeIn;

            match->pad_idx = pad;

        }



        insert_inout(currInputs, match);



        *buf += consume_whitespace(*buf);

        pad++;

    }



    return pad;

}