static void combine_addr(char *buf, size_t len, const char* address,

                         uint16_t port)

{

    

    if (strstr(address, ":")) {

        snprintf(buf, len, "[%s]:%u", address, port);

    } else {

        snprintf(buf, len, "%s:%u", address, port);

    }

}