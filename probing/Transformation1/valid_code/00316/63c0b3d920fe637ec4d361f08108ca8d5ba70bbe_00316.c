static void reverse_dc_prediction(Vp3DecodeContext *s,

                                  int first_fragment,

                                  int fragment_width,

                                  int fragment_height)

{



#define PUL 8

#define PU 4

#define PUR 2

#define PL 1



    int x, y;

    int i = first_fragment;



    int predicted_dc;



    

    int vl, vul, vu, vur;



    

    int l, ul, u, ur;



    

    static const int predictor_transform[16][4] = {

        {  0,  0,  0,  0},

        {  0,  0,  0,128},        

        {  0,  0,128,  0},        

        {  0,  0, 53, 75},        

        {  0,128,  0,  0},        

        {  0, 64,  0, 64},        

        {  0,128,  0,  0},        

        {  0,  0, 53, 75},        

        {128,  0,  0,  0},        

        {  0,  0,  0,128},        

        { 64,  0, 64,  0},        

        {  0,  0, 53, 75},        

        {  0,128,  0,  0},        

       {-104,116,  0,116},        

        { 24, 80, 24,  0},        

       {-104,116,  0,116}         

    };



    

    static const unsigned char compatible_frame[8] = {

        1,    

        0,    

        1,    

        1,    

        1,    

        2,    

        2,    

        1     

    };

    int current_frame_type;



    

    short last_dc[3];



    int transform = 0;



    vul = vu = vur = vl = 0;

    last_dc[0] = last_dc[1] = last_dc[2] = 0;



    

    for (y = 0; y < fragment_height; y++) {



        

        for (x = 0; x < fragment_width; x++, i++) {



            

            if (s->all_fragments[i].coding_method != MODE_COPY) {



                current_frame_type =

                    compatible_frame[s->all_fragments[i].coding_method];



                transform= 0;

                if(x){

                    l= i-1;

                    vl = DC_COEFF(l);

                    if(FRAME_CODED(l) && COMPATIBLE_FRAME(l))

                        transform |= PL;

                }

                if(y){

                    u= i-fragment_width;

                    vu = DC_COEFF(u);

                    if(FRAME_CODED(u) && COMPATIBLE_FRAME(u))

                        transform |= PU;

                    if(x){

                        ul= i-fragment_width-1;

                        vul = DC_COEFF(ul);

                        if(FRAME_CODED(ul) && COMPATIBLE_FRAME(ul))

                            transform |= PUL;

                    }

                    if(x + 1 < fragment_width){

                        ur= i-fragment_width+1;

                        vur = DC_COEFF(ur);

                        if(FRAME_CODED(ur) && COMPATIBLE_FRAME(ur))

                            transform |= PUR;

                    }

                }



                if (transform == 0) {



                    

                    predicted_dc = last_dc[current_frame_type];

                } else {



                    

                    predicted_dc =

                        (predictor_transform[transform][0] * vul) +

                        (predictor_transform[transform][1] * vu) +

                        (predictor_transform[transform][2] * vur) +

                        (predictor_transform[transform][3] * vl);



                    predicted_dc /= 128;



                    

                    if ((transform == 13) || (transform == 15)) {

                        if (FFABS(predicted_dc - vu) > 128)

                            predicted_dc = vu;

                        else if (FFABS(predicted_dc - vl) > 128)

                            predicted_dc = vl;

                        else if (FFABS(predicted_dc - vul) > 128)

                            predicted_dc = vul;

                    }

                }



                

                if(s->coeffs[i].index){

                    *s->next_coeff= s->coeffs[i];

                    s->coeffs[i].index=0;

                    s->coeffs[i].coeff=0;

                    s->coeffs[i].next= s->next_coeff++;

                }

                s->coeffs[i].coeff += predicted_dc;

                

                last_dc[current_frame_type] = DC_COEFF(i);

                if(DC_COEFF(i) && !(s->coeff_counts[i]&127)){

                    s->coeff_counts[i]= 129;



                    s->coeffs[i].next= s->next_coeff;

                    (s->next_coeff++)->next=NULL;

                }

            }

        }

    }

}