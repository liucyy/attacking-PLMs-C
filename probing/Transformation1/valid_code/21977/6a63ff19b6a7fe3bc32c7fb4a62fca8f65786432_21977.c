static int mov_read_tkhd(MOVContext *c, ByteIOContext *pb, MOVAtom atom)

{

    int i;

    int width;

    int height;

    int64_t disp_transform[2];

    int display_matrix[3][2];

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];

    MOVStreamContext *sc = st->priv_data;

    int version = get_byte(pb);



    get_be24(pb); 

    



    if (version == 1) {

        get_be64(pb);

        get_be64(pb);

    } else {

        get_be32(pb); 

        get_be32(pb); 

    }

    st->id = (int)get_be32(pb); 

    get_be32(pb); 



    

    (version == 1) ? get_be64(pb) : get_be32(pb);

    get_be32(pb); 

    get_be32(pb); 



    get_be16(pb); 

    get_be16(pb); 

    get_be16(pb); 

    get_be16(pb); 



    

    

    

    for (i = 0; i < 3; i++) {

        display_matrix[i][0] = get_be32(pb);   

        display_matrix[i][1] = get_be32(pb);   

        get_be32(pb);           

    }



    width = get_be32(pb);       

    height = get_be32(pb);      

    sc->width = width >> 16;

    sc->height = height >> 16;



    

    

    

    if (width && height &&

        (display_matrix[0][0] != 65536 || display_matrix[0][1]           ||

        display_matrix[1][0]           || display_matrix[1][1] != 65536  ||

        display_matrix[2][0]           || display_matrix[2][1])) {

        for (i = 0; i < 2; i++)

            disp_transform[i] =

                (int64_t)  width  * display_matrix[0][i] +

                (int64_t)  height * display_matrix[1][i] +

                ((int64_t) display_matrix[2][i] << 16);



        

        st->sample_aspect_ratio = av_d2q(

            ((double) disp_transform[0] * height) /

            ((double) disp_transform[1] * width), INT_MAX);

    }

    return 0;

}