static gboolean check_old_packet_regular(void *opaque)

{

    CompareState *s = opaque;



    

    colo_old_packet_check(s);



    return TRUE;

}