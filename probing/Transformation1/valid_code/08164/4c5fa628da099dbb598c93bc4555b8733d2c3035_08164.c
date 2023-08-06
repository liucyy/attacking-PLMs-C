static int mov_read_tkhd(MOVContext *c, AVIOContext *pb, MOVAtom atom)

{

    int i;

    int width;

    int height;

    int64_t disp_transform[2];

    int display_matrix[3][3];

    AVStream *st;

    MOVStreamContext *sc;

    int version;

    int flags;



    if (c->fc->nb_streams < 1)

        return 0;

    st = c->fc->streams[c->fc->nb_streams-1];

    sc = st->priv_data;



    version = avio_r8(pb);

    flags = avio_rb24(pb);

    st->disposition |= (flags & MOV_TKHD_FLAG_ENABLED) ? AV_DISPOSITION_DEFAULT : 0;



    if (version == 1) {

        avio_rb64(pb);

        avio_rb64(pb);

    } else {

        avio_rb32(pb); 

        avio_rb32(pb); 

    }

    st->id = (int)avio_rb32(pb); 

    avio_rb32(pb); 



    

    (version == 1) ? avio_rb64(pb) : avio_rb32(pb);

    avio_rb32(pb); 

    avio_rb32(pb); 



    avio_rb16(pb); 

    avio_rb16(pb); 

    avio_rb16(pb); 

    avio_rb16(pb); 



    

    

    

    

    for (i = 0; i < 3; i++) {

        display_matrix[i][0] = avio_rb32(pb);   

        display_matrix[i][1] = avio_rb32(pb);   

        display_matrix[i][2] = avio_rb32(pb);   

    }



    width = avio_rb32(pb);       

    height = avio_rb32(pb);      

    sc->width = width >> 16;

    sc->height = height >> 16;



    

    if (display_matrix[0][0] != (1 << 16) ||

        display_matrix[1][1] != (1 << 16) ||

        display_matrix[2][2] != (1 << 30) ||

        display_matrix[0][1] || display_matrix[0][2] ||

        display_matrix[1][0] || display_matrix[1][2] ||

        display_matrix[2][0] || display_matrix[2][1]) {

        int i, j;



        av_freep(&sc->display_matrix);

        sc->display_matrix = av_malloc(sizeof(int32_t) * 9);

        if (!sc->display_matrix)

            return AVERROR(ENOMEM);



        for (i = 0; i < 3; i++)

            for (j = 0; j < 3; j++)

                sc->display_matrix[i * 3 + j] = display_matrix[j][i];

    }



    

    

    

    if (width && height && sc->display_matrix &&

        av_display_rotation_get(sc->display_matrix) != 0.0f) {

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