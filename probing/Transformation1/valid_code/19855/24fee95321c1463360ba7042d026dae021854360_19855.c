static int update_offset(RTMPContext *rt, int size)

{

    int old_flv_size;



    

    if (rt->flv_off < rt->flv_size) {

        

        old_flv_size  = rt->flv_size;

        rt->flv_size += size + 15;

    } else {

        

        old_flv_size = 0;

        rt->flv_size = size + 15;

        rt->flv_off  = 0;

    }



    return old_flv_size;

}