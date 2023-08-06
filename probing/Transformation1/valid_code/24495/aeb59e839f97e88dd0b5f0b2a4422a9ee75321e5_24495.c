static int execute_ref_pic_marking(H264Context *h, MMCO *mmco, int mmco_count){
    MpegEncContext * const s = &h->s;
    int i, j;
    int current_ref_assigned=0;
    Picture *pic;
    if((s->avctx->debug&FF_DEBUG_MMCO) && mmco_count==0)
        av_log(h->s.avctx, AV_LOG_DEBUG, "no mmco here\n");
    for(i=0; i<mmco_count; i++){
        int structure, frame_num, unref_pic;
        if(s->avctx->debug&FF_DEBUG_MMCO)
            av_log(h->s.avctx, AV_LOG_DEBUG, "mmco:%d %d %d\n", h->mmco[i].opcode, h->mmco[i].short_pic_num, h->mmco[i].long_arg);
        switch(mmco[i].opcode){
        case MMCO_SHORT2UNUSED:
            if(s->avctx->debug&FF_DEBUG_MMCO)
                av_log(h->s.avctx, AV_LOG_DEBUG, "mmco: unref short %d count %d\n", h->mmco[i].short_pic_num, h->short_ref_count);
            frame_num = pic_num_extract(h, mmco[i].short_pic_num, &structure);
            pic = find_short(h, frame_num, &j);
            if (pic) {
                if (unreference_pic(h, pic, structure ^ PICT_FRAME))
                    remove_short_at_index(h, j);
            } else if(s->avctx->debug&FF_DEBUG_MMCO)
                av_log(h->s.avctx, AV_LOG_DEBUG, "mmco: unref short failure\n");
        case MMCO_SHORT2LONG:
            if (FIELD_PICTURE && mmco[i].long_arg < h->long_ref_count &&
                    h->long_ref[mmco[i].long_arg]->frame_num ==
                                              mmco[i].short_pic_num / 2) {
                
                int frame_num = mmco[i].short_pic_num >> FIELD_PICTURE;
                pic= remove_long(h, mmco[i].long_arg);
                if(pic) unreference_pic(h, pic, 0);
                h->long_ref[ mmco[i].long_arg ]= remove_short(h, frame_num);
                if (h->long_ref[ mmco[i].long_arg ]){
                    h->long_ref[ mmco[i].long_arg ]->long_ref=1;
                    h->long_ref_count++;
        case MMCO_LONG2UNUSED:
            j = pic_num_extract(h, mmco[i].long_arg, &structure);
            pic = h->long_ref[j];
            if (pic) {
                if (unreference_pic(h, pic, structure ^ PICT_FRAME))
                    remove_long_at_index(h, j);
            } else if(s->avctx->debug&FF_DEBUG_MMCO)
                av_log(h->s.avctx, AV_LOG_DEBUG, "mmco: unref long failure\n");
        case MMCO_LONG:
            unref_pic = 1;
            if (FIELD_PICTURE && !s->first_field) {
                if (h->long_ref[mmco[i].long_arg] == s->current_picture_ptr) {
                    
                    unref_pic = 0;
                } else if (s->current_picture_ptr->reference) {
                    
            s->current_picture_ptr->reference = PICT_FRAME;
        } else if (s->current_picture_ptr->long_ref) {
            av_log(h->s.avctx, AV_LOG_ERROR, "illegal short term reference "
                                             "assignment for second field "
                                             "in complementary field pair "
                                             "(first field is long term)\n");
            /*
             * First field in reference, but not in any sensible place on our
             * reference lists. This shouldn't happen unless reference
             * handling somewhere else is wrong.
            assert(0);
        current_ref_assigned = 1;
    if(!current_ref_assigned){
        pic= remove_short(h, s->current_picture_ptr->frame_num);
        if(pic){
            unreference_pic(h, pic, 0);
            av_log(h->s.avctx, AV_LOG_ERROR, "illegal short term buffer state detected\n");
        if(h->short_ref_count)
            memmove(&h->short_ref[1], &h->short_ref[0], h->short_ref_count*sizeof(Picture*));
        h->short_ref[0]= s->current_picture_ptr;
        h->short_ref[0]->long_ref=0;
        h->short_ref_count++;
        s->current_picture_ptr->reference |= s->picture_structure;
    print_short_term(h);
    print_long_term(h);
    return 0;