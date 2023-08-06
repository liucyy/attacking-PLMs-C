static void pl050_write(void *opaque, hwaddr offset,

                        uint64_t value, unsigned size)

{

    pl050_state *s = (pl050_state *)opaque;

    switch (offset >> 2) {

    case 0: 

        s->cr = value;

        pl050_update(s, s->pending);

        

        break;

    case 2: 

        

        

        if (s->is_mouse) {

            ps2_write_mouse(s->dev, value);

        } else {

            ps2_write_keyboard(s->dev, value);

        }

        break;

    case 3: 

        s->clk = value;

        return;

    default:

        hw_error("pl050_write: Bad offset %x\n", (int)offset);

    }

}