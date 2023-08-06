ogg_read_header (AVFormatContext * s, AVFormatParameters * ap)
{
    struct ogg *ogg = s->priv_data;
    ogg->curidx = -1;
    
    if (ogg_get_headers (s) < 0){
        return -1;
    }
    
    ogg_get_length (s);
    
    return 0;
}