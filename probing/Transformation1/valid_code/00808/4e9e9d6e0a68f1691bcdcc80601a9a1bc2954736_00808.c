static int parse_bootdevices(char *devices)

{

    

    const char *p;

    int bitmap = 0;



    for (p = devices; *p != '\0'; p++) {

        

        if (*p < 'a' || *p > 'p') {

            fprintf(stderr, "Invalid boot device '%c'\n", *p);

            exit(1);

        }

        if (bitmap & (1 << (*p - 'a'))) {

            fprintf(stderr, "Boot device '%c' was given twice\n", *p);

            exit(1);

        }

        bitmap |= 1 << (*p - 'a');

    }

    return bitmap;

}