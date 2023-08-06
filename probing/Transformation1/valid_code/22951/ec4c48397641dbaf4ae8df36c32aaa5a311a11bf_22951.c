static int io_open_default(AVFormatContext *s, AVIOContext **pb,

                           const char *url, int flags, AVDictionary **options)

{

    return avio_open2(pb, url, flags, &s->interrupt_callback, options);

}