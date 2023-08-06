static QObject *parse_literal(JSONParserContext *ctxt)

{

    QObject *token;



    token = parser_context_pop_token(ctxt);

    assert(token);



    switch (token_get_type(token)) {

    case JSON_STRING:

        return QOBJECT(qstring_from_escaped_str(ctxt, token));

    case JSON_INTEGER: {

        

        int64_t value;



        errno = 0; 

        value = strtoll(token_get_value(token), NULL, 10);

        if (errno != ERANGE) {

            return QOBJECT(qint_from_int(value));

        }

        

    }

    case JSON_FLOAT:

        

        return QOBJECT(qfloat_from_double(strtod(token_get_value(token),

                                                 NULL)));

    default:

        abort();

    }

}