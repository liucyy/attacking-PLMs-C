static void imx_gpt_compute_next_timeout(IMXGPTState *s, bool event)

{

    uint32_t timeout = TIMER_MAX;

    uint32_t count = 0;

    long long limit;



    if (!(s->cr & GPT_CR_EN)) {

        

        return;

    }



    if (event) {

        



        if ((s->cr & GPT_CR_FRR)  && (s->next_timeout != TIMER_MAX)) {

            

            count = imx_gpt_update_count(s);

        }

    } else {

        



        count = imx_gpt_update_count(s);

    }



    



    if (s->ir & GPT_IR_OF1IE) {

        timeout = imx_gpt_find_limit(count, s->ocr1, timeout);

    }

    if (s->ir & GPT_IR_OF2IE) {

        timeout = imx_gpt_find_limit(count, s->ocr2, timeout);

    }

    if (s->ir & GPT_IR_OF3IE) {

        timeout = imx_gpt_find_limit(count, s->ocr3, timeout);

    }



    



    s->next_int = 0;

    if ((s->ir & GPT_IR_OF1IE) && (timeout == s->ocr1)) {

        s->next_int |= GPT_SR_OF1;

    }

    if ((s->ir & GPT_IR_OF2IE) && (timeout == s->ocr2)) {

        s->next_int |= GPT_SR_OF2;

    }

    if ((s->ir & GPT_IR_OF3IE) && (timeout == s->ocr3)) {

        s->next_int |= GPT_SR_OF3;

    }

    if ((s->ir & GPT_IR_ROVIE) && (timeout == TIMER_MAX)) {

        s->next_int |= GPT_SR_ROV;

    }



    

    limit = timeout - imx_gpt_update_count(s);



    if (limit < 0) {

        

        s->sr |= s->next_int;



        imx_gpt_compute_next_timeout(s, event);



        imx_gpt_update_int(s);

    } else {

        

        s->next_timeout = timeout;



        

        ptimer_set_limit(s->timer, limit, 1);

    }

}