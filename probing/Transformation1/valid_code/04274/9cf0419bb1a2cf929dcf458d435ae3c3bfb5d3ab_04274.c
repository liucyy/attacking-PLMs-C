static int mov_read_moov(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    int err;



    err = mov_read_default(c, pb, atom);

    

    

    c->found_moov=1;

    if(c->found_mdat)

        return 1; 

    return 0; 

}