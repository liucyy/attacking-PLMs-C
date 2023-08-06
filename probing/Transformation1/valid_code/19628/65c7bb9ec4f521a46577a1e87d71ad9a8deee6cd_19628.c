static void vc1_overlap_block(MpegEncContext *s, DCTELEM block[64], int n, int do_hor, int do_vert)

{

    int i;



    if(do_hor) { 

    }

    if(do_vert) { 

    }



    for(i = 0; i < 64; i++)

        block[i] += 128;

}