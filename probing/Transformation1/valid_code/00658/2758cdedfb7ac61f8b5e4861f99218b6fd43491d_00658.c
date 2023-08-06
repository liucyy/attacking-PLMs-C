static const AVClass *urlcontext_child_class_next(const AVClass *prev)

{

    URLProtocol *p = NULL;



    

    while (prev && (p = ffurl_protocol_next(p)))

        if (p->priv_data_class == prev)

            break;



    

    while (p = ffurl_protocol_next(p))

        if (p->priv_data_class)

            return p->priv_data_class;

    return NULL;

}