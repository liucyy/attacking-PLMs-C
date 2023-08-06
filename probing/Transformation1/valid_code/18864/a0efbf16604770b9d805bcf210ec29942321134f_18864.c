GList *range_list_insert(GList *list, Range *data)

{

    GList *l;



    

    assert(data->begin < data->end || (data->begin && !data->end));



    

    for (l = list; l && range_compare(l->data, data) < 0; l = l->next) {

    }



    if (!l || range_compare(l->data, data) > 0) {

        

        return g_list_insert_before(list, l, data);

    }



    

    range_extend(l->data, data);

    g_free(data);



    

    while (l->next && range_compare(l->data, l->next->data) == 0) {

        GList *new_l;



        range_extend(l->data, l->next->data);

        g_free(l->next->data);

        new_l = g_list_delete_link(list, l->next);

        assert(new_l == list);

    }



    return list;

}