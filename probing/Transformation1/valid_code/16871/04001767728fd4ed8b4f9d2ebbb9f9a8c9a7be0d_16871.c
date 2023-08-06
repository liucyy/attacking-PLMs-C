static int query_format(struct vf_instance *vf, unsigned int fmt)

{

    

    switch (fmt) {

    case IMGFMT_YV12:

    case IMGFMT_IYUV:

    case IMGFMT_I420:

        return ff_vf_next_query_format(vf, fmt);

    }

    return 0;

}