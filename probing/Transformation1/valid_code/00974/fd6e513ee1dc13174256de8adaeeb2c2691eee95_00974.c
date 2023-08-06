static int mov_read_mdhd(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];



    print_atom("mdhd", atom);



    get_byte(pb); 



    get_byte(pb); get_byte(pb);

    get_byte(pb); 



    get_be32(pb); 

    get_be32(pb); 



    c->streams[c->total_streams]->time_scale = get_be32(pb);



#ifdef DEBUG

    printf("track[%i].time_scale = %i\n", c->fc->nb_streams-1, c->streams[c->total_streams]->time_scale); 

#endif

    get_be32(pb); 



    get_be16(pb); 

    get_be16(pb); 



    return 0;

}