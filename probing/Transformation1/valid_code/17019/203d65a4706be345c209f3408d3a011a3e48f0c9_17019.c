static void imx_gpt_write(void *opaque, hwaddr offset, uint64_t value,

                          unsigned size)

{

    IMXGPTState *s = IMX_GPT(opaque);

    uint32_t oldreg;

    uint32_t reg = offset >> 2;



    DPRINTF("(%s, value = 0x%08x)\n", imx_gpt_reg_name(reg),

            (uint32_t)value);



    switch (reg) {

    case 0:

        oldreg = s->cr;

        s->cr = value & ~0x7c14;

        if (s->cr & GPT_CR_SWR) { 

            

            imx_gpt_reset(DEVICE(s));

        } else {

            

            imx_gpt_set_freq(s);



            if ((oldreg ^ s->cr) & GPT_CR_EN) {

                if (s->cr & GPT_CR_EN) {

                    if (s->cr & GPT_CR_ENMOD) {

                        s->next_timeout = TIMER_MAX;

                        ptimer_set_count(s->timer, TIMER_MAX);

                        imx_gpt_compute_next_timeout(s, false);

                    }

                    ptimer_run(s->timer, 1);

                } else {

                    

                    ptimer_stop(s->timer);

                }

            }

        }

        break;



    case 1: 

        s->pr = value & 0xfff;

        imx_gpt_set_freq(s);

        break;



    case 2: 

        s->sr &= ~(value & 0x3f);

        imx_gpt_update_int(s);

        break;



    case 3: 

        s->ir = value & 0x3f;

        imx_gpt_update_int(s);



        imx_gpt_compute_next_timeout(s, false);



        break;



    case 4: 

        s->ocr1 = value;



        

        if (!(s->cr & GPT_CR_FRR)) {

            s->next_timeout = TIMER_MAX;

            ptimer_set_limit(s->timer, TIMER_MAX, 1);

        }



        

        imx_gpt_compute_next_timeout(s, false);



        break;



    case 5: 

        s->ocr2 = value;



        

        imx_gpt_compute_next_timeout(s, false);



        break;



    case 6: 

        s->ocr3 = value;



        

        imx_gpt_compute_next_timeout(s, false);



        break;



    default:

        IPRINTF("Bad offset %x\n", reg);

        break;

    }

}