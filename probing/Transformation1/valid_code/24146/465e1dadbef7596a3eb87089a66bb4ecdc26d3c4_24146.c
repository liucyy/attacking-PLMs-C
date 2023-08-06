static uint64_t find_any_startcode(ByteIOContext *bc, int64_t pos){

    uint64_t state=0;

    

    if(pos >= 0)

        url_fseek(bc, pos, SEEK_SET); 



    while(bytes_left(bc)){

        state= (state<<8) | get_byte(bc);

        if((state>>56) != 'N')

            continue;

        switch(state){

        case MAIN_STARTCODE:

        case STREAM_STARTCODE:

        case KEYFRAME_STARTCODE:

        case INFO_STARTCODE:

        case INDEX_STARTCODE:

            return state;

        }

    }

    return 0;

}