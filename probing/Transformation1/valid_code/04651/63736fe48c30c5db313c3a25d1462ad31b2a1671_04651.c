int ff_avfilter_graph_config_formats(AVFilterGraph *graph, AVClass *log_ctx)

{

    int ret;



    

    if ((ret = query_formats(graph, log_ctx)) < 0)

        return ret;



    

    pick_formats(graph);



    return 0;

}