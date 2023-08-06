static int mxf_get_sorted_table_segments(MXFContext *mxf, int *nb_sorted_segments, MXFIndexTableSegment ***sorted_segments)

{

    int i, j, nb_segments = 0;

    MXFIndexTableSegment **unsorted_segments;

    int last_body_sid = -1, last_index_sid = -1, last_index_start = -1;



    

    for (i = 0; i < mxf->metadata_sets_count; i++)

        if (mxf->metadata_sets[i]->type == IndexTableSegment)

            nb_segments++;



    if (!nb_segments)

        return AVERROR_INVALIDDATA;



    *sorted_segments  = av_mallocz(nb_segments * sizeof(**sorted_segments));

    unsorted_segments = av_mallocz(nb_segments * sizeof(*unsorted_segments));

    if (!sorted_segments || !unsorted_segments) {

        av_freep(sorted_segments);

        av_free(unsorted_segments);

        return AVERROR(ENOMEM);

    }



    for (i = j = 0; i < mxf->metadata_sets_count; i++)

        if (mxf->metadata_sets[i]->type == IndexTableSegment)

            unsorted_segments[j++] = (MXFIndexTableSegment*)mxf->metadata_sets[i];



    *nb_sorted_segments = 0;



    

    for (i = 0; i < nb_segments; i++) {

        int best = -1, best_body_sid = -1, best_index_sid = -1, best_index_start = -1;



        for (j = 0; j < nb_segments; j++) {

            MXFIndexTableSegment *s = unsorted_segments[j];



            

            if ((i == 0     || s->body_sid > last_body_sid || s->index_sid > last_index_sid || s->index_start_position > last_index_start) &&

                (best == -1 || s->body_sid < best_body_sid || s->index_sid < best_index_sid || s->index_start_position < best_index_start)) {

                best             = j;

                best_body_sid    = s->body_sid;

                best_index_sid   = s->index_sid;

                best_index_start = s->index_start_position;

            }

        }



        

        if (best == -1)

            break;



        (*sorted_segments)[(*nb_sorted_segments)++] = unsorted_segments[best];

        last_body_sid    = best_body_sid;

        last_index_sid   = best_index_sid;

        last_index_start = best_index_start;

    }



    av_free(unsorted_segments);



    return 0;

}