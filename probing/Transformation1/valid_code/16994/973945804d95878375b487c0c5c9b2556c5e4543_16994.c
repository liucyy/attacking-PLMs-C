static void tcx_invalidate_cursor_position(TCXState *s)

{

    int ymin, ymax, start, end;



    

    ymin = s->cursy;

    if (ymin >= s->height) {

        return;

    }

    ymax = MIN(s->height, ymin + 32);

    start = ymin * 1024;

    end   = ymax * 1024;



    memory_region_set_dirty(&s->vram_mem, start, end-start);

}