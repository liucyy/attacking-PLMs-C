static void cpu_common_parse_features(const char *typename, char *features,

                                      Error **errp)

{

    char *featurestr; 

    char *val;

    static bool cpu_globals_initialized;



    

    if (cpu_globals_initialized) {

        return;

    }

    cpu_globals_initialized = true;



    featurestr = features ? strtok(features, ",") : NULL;



    while (featurestr) {

        val = strchr(featurestr, '=');

        if (val) {

            GlobalProperty *prop = g_new0(typeof(*prop), 1);

            *val = 0;

            val++;

            prop->driver = typename;

            prop->property = g_strdup(featurestr);

            prop->value = g_strdup(val);

            prop->errp = &error_fatal;

            qdev_prop_register_global(prop);

        } else {

            error_setg(errp, "Expected key=value format, found %s.",

                       featurestr);

            return;

        }

        featurestr = strtok(NULL, ",");

    }

}