static void dump_map_entry(OutputFormat output_format, MapEntry *e,

                           MapEntry *next)

{

    switch (output_format) {

    case OFORMAT_HUMAN:

        if ((e->flags & BDRV_BLOCK_DATA) &&

            !(e->flags & BDRV_BLOCK_OFFSET_VALID)) {

            error_report("File contains external, encrypted or compressed clusters.");

            exit(1);

        }

        if ((e->flags & (BDRV_BLOCK_DATA|BDRV_BLOCK_ZERO)) == BDRV_BLOCK_DATA) {

            printf("%#-16"PRIx64"%#-16"PRIx64"%#-16"PRIx64"%s\n",

                   e->start, e->length, e->offset, e->bs->filename);

        }

        

        if (next &&

            (next->flags & (BDRV_BLOCK_DATA|BDRV_BLOCK_ZERO)) != BDRV_BLOCK_DATA) {

            next->flags &= ~BDRV_BLOCK_DATA;

            next->flags |= BDRV_BLOCK_ZERO;

        }

        break;

    case OFORMAT_JSON:

        printf("%s{ \"start\": %"PRId64", \"length\": %"PRId64", \"depth\": %d,"

               " \"zero\": %s, \"data\": %s",

               (e->start == 0 ? "[" : ",\n"),

               e->start, e->length, e->depth,

               (e->flags & BDRV_BLOCK_ZERO) ? "true" : "false",

               (e->flags & BDRV_BLOCK_DATA) ? "true" : "false");

        if (e->flags & BDRV_BLOCK_OFFSET_VALID) {

            printf(", 'offset': %"PRId64"", e->offset);

        }

        putchar('}');



        if (!next) {

            printf("]\n");

        }

        break;

    }

}