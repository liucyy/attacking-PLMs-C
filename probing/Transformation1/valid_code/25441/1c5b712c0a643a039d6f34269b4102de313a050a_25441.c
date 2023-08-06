static void interpolate_refplane(DiracContext *s, DiracFrame *ref, int plane, int width, int height)

{

    

    int i, edge = EDGE_WIDTH/2;



    ref->hpel[plane][0] = ref->avframe->data[plane];

    s->mpvencdsp.draw_edges(ref->hpel[plane][0], ref->avframe->linesize[plane], width, height, edge, edge, EDGE_TOP | EDGE_BOTTOM); 



    

    if (!s->mv_precision)

        return;



    for (i = 1; i < 4; i++) {

        if (!ref->hpel_base[plane][i])

            ref->hpel_base[plane][i] = av_malloc((height+2*edge) * ref->avframe->linesize[plane] + 32);

        

        ref->hpel[plane][i] = ref->hpel_base[plane][i] + edge*ref->avframe->linesize[plane] + 16;

    }



    if (!ref->interpolated[plane]) {

        s->diracdsp.dirac_hpel_filter(ref->hpel[plane][1], ref->hpel[plane][2],

                                      ref->hpel[plane][3], ref->hpel[plane][0],

                                      ref->avframe->linesize[plane], width, height);

        s->mpvencdsp.draw_edges(ref->hpel[plane][1], ref->avframe->linesize[plane], width, height, edge, edge, EDGE_TOP | EDGE_BOTTOM);

        s->mpvencdsp.draw_edges(ref->hpel[plane][2], ref->avframe->linesize[plane], width, height, edge, edge, EDGE_TOP | EDGE_BOTTOM);

        s->mpvencdsp.draw_edges(ref->hpel[plane][3], ref->avframe->linesize[plane], width, height, edge, edge, EDGE_TOP | EDGE_BOTTOM);

    }

    ref->interpolated[plane] = 1;

}