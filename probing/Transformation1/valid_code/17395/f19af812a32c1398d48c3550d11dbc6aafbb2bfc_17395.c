static int adx_encode_header(AVCodecContext *avctx,unsigned char *buf,size_t bufsize)

{

#if 0

	struct {

		uint32_t offset; 

		unsigned char unknown1[3]; 

		unsigned char channel; 

		uint32_t freq;

		uint32_t size;

		uint32_t unknown2; 

		uint32_t unknown3; 

		uint32_t unknown4; 



	

	} adxhdr; 

	

#endif

	write_long(buf+0x00,0x80000000|0x20);

	write_long(buf+0x04,0x03120400|avctx->channels);

	write_long(buf+0x08,avctx->sample_rate);

	write_long(buf+0x0c,0); 

	write_long(buf+0x10,0x01040300);

	write_long(buf+0x14,0x00000000);

	write_long(buf+0x18,0x00000000);

	memcpy(buf+0x1c,"\0\0(c)CRI",8);

	return 0x20+4;

}