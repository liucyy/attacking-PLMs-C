static int graph_config_formats(AVFilterGraph *graph, AVClass *log_ctx)

{

    int ret;



    

    if ((ret = query_formats(graph, log_ctx)) < 0)

        return ret;



    

    reduce_formats(graph);



    

    swap_sample_fmts(graph);

    swap_samplerates(graph);

    swap_channel_layouts(graph);



    if ((ret = pick_formats(graph)) < 0)

        return ret;



    return 0;

}