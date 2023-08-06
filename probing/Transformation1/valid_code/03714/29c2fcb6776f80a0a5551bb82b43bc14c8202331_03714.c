static void lms_update(WmallDecodeCtx *s, int ich, int ilms, int16_t input, int16_t pred)

{

    int16_t icoef;

    int recent = s->cdlms[ich][ilms].recent;

    int16_t range = 1 << (s->bits_per_sample - 1);

    int bps = s->bits_per_sample > 16 ? 4 : 2; 



    if (input > pred) {

        for (icoef = 0; icoef < s->cdlms[ich][ilms].order; icoef++)

            s->cdlms[ich][ilms].coefs[icoef] +=

                s->cdlms[ich][ilms].lms_updates[icoef + recent];

    } else {

        for (icoef = 0; icoef < s->cdlms[ich][ilms].order; icoef++)

            s->cdlms[ich][ilms].coefs[icoef] -=

                s->cdlms[ich][ilms].lms_updates[icoef];     

    }

    s->cdlms[ich][ilms].recent--;

    s->cdlms[ich][ilms].lms_prevvalues[recent] = av_clip(input, -range, range - 1);



    if (input > pred)

        s->cdlms[ich][ilms].lms_updates[recent] = s->update_speed[ich];

    else if (input < pred)

        s->cdlms[ich][ilms].lms_updates[recent] = -s->update_speed[ich];



    

    s->cdlms[ich][ilms].lms_updates[recent + s->cdlms[ich][ilms].order >> 4] >>= 2;

    s->cdlms[ich][ilms].lms_updates[recent + s->cdlms[ich][ilms].order >> 3] >>= 1;

    



    if (s->cdlms[ich][ilms].recent == 0) {

        

        memcpy(s->cdlms[ich][ilms].lms_prevvalues + s->cdlms[ich][ilms].order,

               s->cdlms[ich][ilms].lms_prevvalues,

               bps * s->cdlms[ich][ilms].order);

        memcpy(s->cdlms[ich][ilms].lms_updates + s->cdlms[ich][ilms].order,

               s->cdlms[ich][ilms].lms_updates,

               bps * s->cdlms[ich][ilms].order);

        s->cdlms[ich][ilms].recent = s->cdlms[ich][ilms].order;

    }

}