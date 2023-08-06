static void fill_buffer(ByteIOContext *s)

{

    int len;



    

    if (s->eof_reached)

        return;



    if(s->update_checksum){

        if(s->buf_end > s->checksum_ptr)

            s->checksum= s->update_checksum(s->checksum, s->checksum_ptr, s->buf_end - s->checksum_ptr);

        s->checksum_ptr= s->buffer;

    }



    len = s->read_packet(s->opaque, s->buffer, s->buffer_size);

    if (len <= 0) {

        

        s->eof_reached = 1;

        if(len<0)

            s->error= len;

    } else {

        s->pos += len;

        s->buf_ptr = s->buffer;

        s->buf_end = s->buffer + len;

    }

}