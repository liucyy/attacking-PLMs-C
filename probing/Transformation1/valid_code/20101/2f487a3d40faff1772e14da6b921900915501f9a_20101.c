static int vnc_update_client(VncState *vs, int has_dirty, bool sync)

{

    if (vs->need_update && vs->csock != -1) {

        VncDisplay *vd = vs->vd;

        VncJob *job;

        int y;

        int height;

        int n = 0;



        if (vs->output.offset && !vs->audio_cap && !vs->force_update)

            

            return 0;



        if (!has_dirty && !vs->audio_cap && !vs->force_update)

            return 0;



        

        job = vnc_job_new(vs);



        height = MIN(pixman_image_get_height(vd->server), vs->client_height);



        y = 0;

        for (;;) {

            int x, h;

            unsigned long x2;

            unsigned long offset = find_next_bit((unsigned long *) &vs->dirty,

                                                 height * VNC_DIRTY_BPL(vs),

                                                 y * VNC_DIRTY_BPL(vs));

            if (offset == height * VNC_DIRTY_BPL(vs)) {

                

                break;

            }

            y = offset / VNC_DIRTY_BPL(vs);

            x = offset % VNC_DIRTY_BPL(vs);

            x2 = find_next_zero_bit((unsigned long *) &vs->dirty[y],

                                    VNC_DIRTY_BPL(vs), x);

            bitmap_clear(vs->dirty[y], x, x2 - x);

            h = find_and_clear_dirty_height(vs, y, x, x2, height);

            n += vnc_job_add_rect(job, x * VNC_DIRTY_PIXELS_PER_BIT, y,

                                  (x2 - x) * VNC_DIRTY_PIXELS_PER_BIT, h);

        }



        vnc_job_push(job);

        vs->force_update = 0;

        return n;

    }



    if (vs->csock == -1) {

        vnc_disconnect_finish(vs);

    } else if (sync) {

        vnc_jobs_join(vs);

    }



    return 0;

}