const DVprofile* avpriv_dv_frame_profile(const DVprofile *sys,

                                  const uint8_t* frame, unsigned buf_size)

{

   int i;



   int dsf = (frame[3] & 0x80) >> 7;



   int stype = frame[80*5 + 48 + 3] & 0x1f;



   

   if (dsf == 1 && stype == 0 && frame[4] & 0x07 ) {

       return &dv_profiles[2];

   }



   for (i=0; i<FF_ARRAY_ELEMS(dv_profiles); i++)

       if (dsf == dv_profiles[i].dsf && stype == dv_profiles[i].video_stype)

           return &dv_profiles[i];



   

   if (sys && buf_size == sys->frame_size)

       return sys;



   return NULL;

}