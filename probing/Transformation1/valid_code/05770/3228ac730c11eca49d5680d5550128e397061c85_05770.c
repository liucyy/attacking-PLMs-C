av_cold void ff_vc2enc_free_transforms(VC2TransformContext *s)

{

    av_freep(&s->buffer);

}