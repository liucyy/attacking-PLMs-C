static int mov_read_stsd(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];

    MOVStreamContext *sc = (MOVStreamContext *)st->priv_data;

    int entries, frames_per_sample;

    uint32_t format;



    print_atom("stsd", atom);



    get_byte(pb); 

    get_byte(pb); get_byte(pb); get_byte(pb); 



    entries = get_be32(pb);



    while(entries--) {

        enum CodecID id;

	int size = get_be32(pb); 

        format = get_le32(pb); 



        get_be32(pb); 

        get_be16(pb); 

        get_be16(pb); 



        

        id = codec_get_id(mov_video_tags, format);

        if (id >= 0) {

            AVCodec *codec;

	    codec = avcodec_find_decoder(id);

            if (codec)

		st->codec.codec_type = codec->type;

        }

#ifdef DEBUG

        printf("size=%d 4CC= %c%c%c%c codec_type=%d\n",

               size,

               (format >> 0) & 0xff,

               (format >> 8) & 0xff,

               (format >> 16) & 0xff,

               (format >> 24) & 0xff,

               st->codec.codec_type);

#endif

	st->codec.codec_tag = format;

	if(st->codec.codec_type==CODEC_TYPE_VIDEO) {

	    MOV_atom_t a = { 0, 0, 0 };

            st->codec.codec_id = id;

            get_be16(pb); 

            get_be16(pb); 

            get_be32(pb); 

            get_be32(pb); 

            get_be32(pb); 

            st->codec.width = get_be16(pb); 

            st->codec.height = get_be16(pb); 

#if 1

            if (st->codec.codec_id == CODEC_ID_MPEG4) {

                

                st->codec.width = 0;

                st->codec.height = 0;

            }

#endif

            get_be32(pb); 

            get_be32(pb); 

            get_be32(pb); 

            frames_per_sample = get_be16(pb); 

#ifdef DEBUG

	    printf("frames/samples = %d\n", frames_per_sample);

#endif

	    get_buffer(pb, (uint8_t *)st->codec.codec_name, 32); 



	    st->codec.bits_per_sample = get_be16(pb); 

            st->codec.color_table_id = get_be16(pb); 



            st->codec.frame_rate      = 25;

            st->codec.frame_rate_base = 1;



	    size -= (16+8*4+2+32+2*2);

#if 0

	    while (size >= 8) {

		MOV_atom_t a;

                int64_t start_pos;



		a.size = get_be32(pb);

		a.type = get_le32(pb);

		size -= 8;

#ifdef DEBUG

                printf("VIDEO: atom_type=%c%c%c%c atom.size=%Ld size_left=%d\n",

                       (a.type >> 0) & 0xff,

                       (a.type >> 8) & 0xff,

                       (a.type >> 16) & 0xff,

                       (a.type >> 24) & 0xff,

		       a.size, size);

#endif

                start_pos = url_ftell(pb);



		switch(a.type) {

                case MKTAG('e', 's', 'd', 's'):

                    {

                        int tag, len;

                        

                        get_be32(pb); 

			len = mov_mp4_read_descr(pb, &tag);

                        if (tag == 0x03) {

                            

                            get_be16(pb); 

                            get_byte(pb); 

			    len = mov_mp4_read_descr(pb, &tag);

                            if (tag != 0x04)

                                goto fail;

                            

                            get_byte(pb); 

                            get_be32(pb); 

			    get_be32(pb); 

                            get_be32(pb); 

                            len = mp4_read_descr(pb, &tag);

                            if (tag != 0x05)

                                goto fail;

                            

#ifdef DEBUG

                            printf("Specific MPEG4 header len=%d\n", len);

#endif

                            sc->header_data = av_mallocz(len);

                            if (sc->header_data) {

                                get_buffer(pb, sc->header_data, len);

				sc->header_len = len;

                            }

                        }

                        

                    }

                    break;

                default:

                    break;

                }

	    fail:

		printf("ATOMENEWSIZE %Ld   %d\n", atom.size, url_ftell(pb) - start_pos);

		if (atom.size > 8) {

		    url_fskip(pb, (atom.size - 8) -

			      ((url_ftell(pb) - start_pos)));

		    size -= atom.size - 8;

		}

	    }

            if (size > 0) {

                

                url_fskip(pb, size);

            }

#else

            a.size = size;

	    mov_read_default(c, pb, a);

#endif

	} else {

            get_be16(pb); 

            get_be16(pb); 

            get_be32(pb); 



            st->codec.channels = get_be16(pb);		

	    st->codec.bits_per_sample = get_be16(pb);	



	    st->codec.codec_id = codec_get_id(mov_audio_tags, format);

            

            get_be16(pb); 

            get_be16(pb); 



            st->codec.sample_rate = ((get_be32(pb) >> 16));

	    



	    switch (st->codec.codec_id) {

	    case CODEC_ID_PCM_S16BE:

		if (st->codec.bits_per_sample == 8)

		    st->codec.codec_id = CODEC_ID_PCM_S8;

                

	    case CODEC_ID_PCM_U8:

		st->codec.bit_rate = st->codec.sample_rate * 8;

		break;

	    default:

                ;

	    }

	    get_be32(pb); 

	    get_be32(pb); 

            get_be32(pb); 

            get_be32(pb); 



	    {

		MOV_atom_t a = { format, url_ftell(pb), size - (16 + 20 + 16 + 8) };

		mov_read_default(c, pb, a);

	    }

        }

    }



    return 0;

}