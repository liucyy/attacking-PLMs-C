static void mov_metadata_creation_time(AVDictionary **metadata, int64_t time)
{
    if (time) {
        if(time >= 2082844800)
            time -= 2082844800;  
        avpriv_dict_set_timestamp(metadata, "creation_time", time * 1000000);