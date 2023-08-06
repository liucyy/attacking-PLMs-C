static int put_v(ByteIOContext *bc, uint64_t val)

{

    int i;







    if (bytes_left(bc) < 1)

	return -1;



    val &= 0x7FFFFFFFFFFFFFFFULL; 

    i= get_length(val);



    for (i-=7; i>0; i-=7){

	put_byte(bc, 0x80 | (val>>i));

    }



    put_byte(bc, val&0x7f);



    return 0;

}