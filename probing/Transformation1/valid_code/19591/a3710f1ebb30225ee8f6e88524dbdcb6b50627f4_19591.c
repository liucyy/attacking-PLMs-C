static ChannelElement *get_che(AACContext *ac, int type, int elem_id)
{
    
    if (!ac->oc[1].m4ac.chan_config) {
        return ac->tag_che_map[type][elem_id];
    }
    
    if (!ac->tags_mapped && type == TYPE_CPE && ac->oc[1].m4ac.chan_config == 1) {
        uint8_t layout_map[MAX_ELEM_ID*4][3];
        int layout_map_tags;
        push_output_configuration(ac);
        if (set_default_channel_config(ac->avctx, layout_map, &layout_map_tags,
                                       2) < 0)
            return NULL;
        if (output_configure(ac, layout_map, layout_map_tags,
                             2, OC_TRIAL_FRAME) < 0)
            return NULL;
        ac->oc[1].m4ac.chan_config = 2;
    }
    
    if (!ac->tags_mapped && type == TYPE_SCE && ac->oc[1].m4ac.chan_config == 2 && 0) {
        uint8_t layout_map[MAX_ELEM_ID*4][3];
        int layout_map_tags;
        push_output_configuration(ac);
        av_log(ac->avctx, AV_LOG_DEBUG, "stereo with SCE\n");
        if (set_default_channel_config(ac->avctx, layout_map, &layout_map_tags,
                                       1) < 0)
            return NULL;
        if (output_configure(ac, layout_map, layout_map_tags,
                             1, OC_TRIAL_FRAME) < 0)
            return NULL;
        ac->oc[1].m4ac.chan_config = 1;
    }
    
    switch (ac->oc[1].m4ac.chan_config) {
    case 7:
        if (ac->tags_mapped == 3 && type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][2];
        }
    case 6:
        
        if (ac->tags_mapped == tags_per_config[ac->oc[1].m4ac.chan_config] - 1 && (type == TYPE_LFE || type == TYPE_SCE)) {
            ac->tags_mapped++;
            return ac->tag_che_map[type][elem_id] = ac->che[TYPE_LFE][0];
        }
    case 5:
        if (ac->tags_mapped == 2 && type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][1];
        }
    case 4:
        if (ac->tags_mapped == 2 && ac->oc[1].m4ac.chan_config == 4 && type == TYPE_SCE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_SCE][elem_id] = ac->che[TYPE_SCE][1];
        }
    case 3:
    case 2:
        if (ac->tags_mapped == (ac->oc[1].m4ac.chan_config != 2) && type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][0];
        } else if (ac->oc[1].m4ac.chan_config == 2) {
            return NULL;
        }
    case 1:
        if (!ac->tags_mapped && type == TYPE_SCE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_SCE][elem_id] = ac->che[TYPE_SCE][0];
        }
    default:
        return NULL;
    }
}