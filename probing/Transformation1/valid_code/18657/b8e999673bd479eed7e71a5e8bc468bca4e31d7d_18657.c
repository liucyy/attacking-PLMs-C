void spapr_cpu_parse_features(sPAPRMachineState *spapr)

{

    

    gchar **inpieces;

    gchar *newprops;

    int i, j;

    gchar *compat_str = NULL;



    inpieces = g_strsplit(MACHINE(spapr)->cpu_model, ",", 0);



    

    i = 1;

    j = 1;

    while (inpieces[i]) {

        if (g_str_has_prefix(inpieces[i], "compat=")) {

            

            g_free(compat_str);

            compat_str = inpieces[i];

        } else {

            j++;

        }



        i++;

        

        inpieces[j] = inpieces[i];

    }



    if (compat_str) {

        char *val = compat_str + strlen("compat=");



        object_property_set_str(OBJECT(spapr), val, "max-cpu-compat",

                                &error_fatal);



    }



    newprops = g_strjoinv(",", inpieces);

    cpu_parse_cpu_model(TYPE_POWERPC_CPU, newprops);

    g_free(newprops);

    g_strfreev(inpieces);

}