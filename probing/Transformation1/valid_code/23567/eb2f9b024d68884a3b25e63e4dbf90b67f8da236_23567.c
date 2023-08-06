static void vmsvga_update_display(void *opaque)

{

    struct vmsvga_state_s *s = opaque;

    DisplaySurface *surface;

    bool dirty = false;



    if (!s->enable) {

        s->vga.update(&s->vga);

        return;

    }



    vmsvga_check_size(s);

    surface = qemu_console_surface(s->vga.con);



    vmsvga_fifo_run(s);

    vmsvga_update_rect_flush(s);



    

    if (memory_region_is_logging(&s->vga.vram)) {

        vga_sync_dirty_bitmap(&s->vga);

        dirty = memory_region_get_dirty(&s->vga.vram, 0,

            surface_stride(surface) * surface_height(surface),

            DIRTY_MEMORY_VGA);

    }

    if (s->invalidated || dirty) {

        s->invalidated = 0;

        memcpy(surface_data(surface), s->vga.vram_ptr,

               surface_stride(surface) * surface_height(surface));

        dpy_gfx_update(s->vga.con, 0, 0,

                   surface_width(surface), surface_height(surface));

    }

    if (dirty) {

        memory_region_reset_dirty(&s->vga.vram, 0,

            surface_stride(surface) * surface_height(surface),

            DIRTY_MEMORY_VGA);

    }

}