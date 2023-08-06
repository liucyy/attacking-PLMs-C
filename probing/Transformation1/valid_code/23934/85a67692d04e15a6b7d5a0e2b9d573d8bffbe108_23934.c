static gboolean pty_chr_timer(gpointer opaque)

{

    struct CharDriverState *chr = opaque;

    PtyCharDriver *s = chr->opaque;



    if (s->connected) {

        goto out;

    }

    if (s->polling) {

        

        pty_chr_state(chr, 1);

        goto out;

    }



    

    pty_chr_update_read_handler(chr);



out:

    return FALSE;

}