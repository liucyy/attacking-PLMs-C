int MP3lame_encode_frame(AVCodecContext *avctx,

                     unsigned char *frame, int buf_size, void *data)

{

	Mp3AudioContext *s = avctx->priv_data;

	int num, i;









	

	if (s->stereo) {

		num = lame_encode_buffer_interleaved(s->gfp, data,

			MPA_FRAME_SIZE, frame, buf_size);

	} else {

		num = lame_encode_buffer(s->gfp, data, data, MPA_FRAME_SIZE,

			frame, buf_size);





	}



	return num;

}