static int put_packetheader(NUTContext *nut, ByteIOContext *bc, int max_size)

{

    put_flush_packet(bc);

    nut->last_packet_start= nut->packet_start;

    nut->packet_start+= nut->written_packet_size;

    nut->packet_size_pos = url_ftell(bc);

    nut->written_packet_size = max_size;



    

    put_v(bc, nut->written_packet_size); 

    put_v(bc, nut->packet_start - nut->last_packet_start); 



    return 0;

}