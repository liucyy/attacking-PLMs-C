static int vorbis_parse_setup_hdr_floors(vorbis_context *vc)

{

    GetBitContext *gb = &vc->gb;

    int i,j,k;



    vc->floor_count = get_bits(gb, 6) + 1;



    vc->floors = av_mallocz(vc->floor_count * sizeof(*vc->floors));



    for (i = 0; i < vc->floor_count; ++i) {

        vorbis_floor *floor_setup = &vc->floors[i];



        floor_setup->floor_type = get_bits(gb, 16);



        av_dlog(NULL, " %d. floor type %d \n", i, floor_setup->floor_type);



        if (floor_setup->floor_type == 1) {

            int maximum_class = -1;

            unsigned rangebits, rangemax, floor1_values = 2;



            floor_setup->decode = vorbis_floor1_decode;



            floor_setup->data.t1.partitions = get_bits(gb, 5);



            av_dlog(NULL, " %d.floor: %d partitions \n",

                    i, floor_setup->data.t1.partitions);



            for (j = 0; j < floor_setup->data.t1.partitions; ++j) {

                floor_setup->data.t1.partition_class[j] = get_bits(gb, 4);

                if (floor_setup->data.t1.partition_class[j] > maximum_class)

                    maximum_class = floor_setup->data.t1.partition_class[j];



                av_dlog(NULL, " %d. floor %d partition class %d \n",

                        i, j, floor_setup->data.t1.partition_class[j]);






            av_dlog(NULL, " maximum class %d \n", maximum_class);



            for (j = 0; j <= maximum_class; ++j) {

                floor_setup->data.t1.class_dimensions[j] = get_bits(gb, 3) + 1;

                floor_setup->data.t1.class_subclasses[j] = get_bits(gb, 2);



                av_dlog(NULL, " %d floor %d class dim: %d subclasses %d \n", i, j,

                        floor_setup->data.t1.class_dimensions[j],

                        floor_setup->data.t1.class_subclasses[j]);



                if (floor_setup->data.t1.class_subclasses[j]) {

                    GET_VALIDATED_INDEX(floor_setup->data.t1.class_masterbook[j], 8, vc->codebook_count)



                    av_dlog(NULL, "   masterbook: %d \n", floor_setup->data.t1.class_masterbook[j]);




                for (k = 0; k < (1 << floor_setup->data.t1.class_subclasses[j]); ++k) {

                    int16_t bits = get_bits(gb, 8) - 1;

                    if (bits != -1)

                        VALIDATE_INDEX(bits, vc->codebook_count)

                    floor_setup->data.t1.subclass_books[j][k] = bits;



                    av_dlog(NULL, "    book %d. : %d \n", k, floor_setup->data.t1.subclass_books[j][k]);





            floor_setup->data.t1.multiplier = get_bits(gb, 2) + 1;

            floor_setup->data.t1.x_list_dim = 2;



            for (j = 0; j < floor_setup->data.t1.partitions; ++j)

                floor_setup->data.t1.x_list_dim+=floor_setup->data.t1.class_dimensions[floor_setup->data.t1.partition_class[j]];



            floor_setup->data.t1.list = av_mallocz(floor_setup->data.t1.x_list_dim *

                                                   sizeof(*floor_setup->data.t1.list));





            rangebits = get_bits(gb, 4);

            rangemax = (1 << rangebits);

            if (rangemax > vc->blocksize[1] / 2) {


                       "Floor value is too large for blocksize: %u (%"PRIu32")\n",

                       rangemax, vc->blocksize[1] / 2);



            floor_setup->data.t1.list[0].x = 0;

            floor_setup->data.t1.list[1].x = rangemax;



            for (j = 0; j < floor_setup->data.t1.partitions; ++j) {

                for (k = 0; k < floor_setup->data.t1.class_dimensions[floor_setup->data.t1.partition_class[j]]; ++k, ++floor1_values) {

                    floor_setup->data.t1.list[floor1_values].x = get_bits(gb, rangebits);



                    av_dlog(NULL, " %u. floor1 Y coord. %d\n", floor1_values,

                            floor_setup->data.t1.list[floor1_values].x);







            if (ff_vorbis_ready_floor1_list(vc->avccontext,

                                            floor_setup->data.t1.list,

                                            floor_setup->data.t1.x_list_dim)) {



        } else if (floor_setup->floor_type == 0) {

            unsigned max_codebook_dim = 0;



            floor_setup->decode = vorbis_floor0_decode;



            floor_setup->data.t0.order          = get_bits(gb,  8);

            floor_setup->data.t0.rate           = get_bits(gb, 16);

            floor_setup->data.t0.bark_map_size  = get_bits(gb, 16);






            floor_setup->data.t0.amplitude_bits = get_bits(gb,  6);

            

            if (floor_setup->data.t0.amplitude_bits == 0) {


                       "Floor 0 amplitude bits is 0.\n");



            floor_setup->data.t0.amplitude_offset = get_bits(gb, 8);

            floor_setup->data.t0.num_books        = get_bits(gb, 4) + 1;



            

            floor_setup->data.t0.book_list =

                av_malloc(floor_setup->data.t0.num_books);

            if (!floor_setup->data.t0.book_list)

                return AVERROR(ENOMEM);

            

            {

                int idx;

                unsigned book_idx;

                for (idx = 0; idx < floor_setup->data.t0.num_books; ++idx) {

                    GET_VALIDATED_INDEX(book_idx, 8, vc->codebook_count)

                    floor_setup->data.t0.book_list[idx] = book_idx;

                    if (vc->codebooks[book_idx].dimensions > max_codebook_dim)

                        max_codebook_dim = vc->codebooks[book_idx].dimensions;





            create_map(vc, i);



            

            floor_setup->data.t0.lsp =

                av_malloc((floor_setup->data.t0.order + 1 + max_codebook_dim)

                          * sizeof(*floor_setup->data.t0.lsp));

            if (!floor_setup->data.t0.lsp)

                return AVERROR(ENOMEM);



            

            av_dlog(NULL, "floor0 order: %u\n", floor_setup->data.t0.order);

            av_dlog(NULL, "floor0 rate: %u\n", floor_setup->data.t0.rate);

            av_dlog(NULL, "floor0 bark map size: %u\n",

                    floor_setup->data.t0.bark_map_size);

            av_dlog(NULL, "floor0 amplitude bits: %u\n",

                    floor_setup->data.t0.amplitude_bits);

            av_dlog(NULL, "floor0 amplitude offset: %u\n",

                    floor_setup->data.t0.amplitude_offset);

            av_dlog(NULL, "floor0 number of books: %u\n",

                    floor_setup->data.t0.num_books);

            av_dlog(NULL, "floor0 book list pointer: %p\n",

                    floor_setup->data.t0.book_list);

            {

                int idx;

                for (idx = 0; idx < floor_setup->data.t0.num_books; ++idx) {

                    av_dlog(NULL, "  Book %d: %u\n", idx + 1,

                            floor_setup->data.t0.book_list[idx]);



        } else {

            av_log(vc->avccontext, AV_LOG_ERROR, "Invalid floor type!\n");




    return 0;