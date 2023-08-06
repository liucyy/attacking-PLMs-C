static void json_emit_element(QJSON *json, const char *name)

{

    

    if (json->omit_comma) {

        json->omit_comma = false;

    } else {

        qstring_append(json->str, ", ");

    }



    if (name) {

        qstring_append(json->str, "\"");

        qstring_append(json->str, name);

        qstring_append(json->str, "\" : ");

    }

}