static bool object_create_initial(const char *type)

{

    if (g_str_equal(type, "rng-egd")) {

        return false;

    }



    

    if (g_str_equal(type, "filter-buffer") ||

        g_str_equal(type, "filter-dump") ||

        g_str_equal(type, "filter-mirror") ||

        g_str_equal(type, "filter-redirector") ||

        g_str_equal(type, "colo-compare") ||

        g_str_equal(type, "filter-rewriter")) {

        return false;

    }



    

    if (g_str_has_prefix(type, "memory-backend-")) {

        return false;

    }



    return true;

}