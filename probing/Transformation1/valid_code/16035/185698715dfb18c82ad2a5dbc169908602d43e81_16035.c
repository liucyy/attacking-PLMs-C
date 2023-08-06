static unsigned int PerformComparison(const unsigned int opcode)

{

   FPA11 *fpa11 = GET_FPA11();

   unsigned int Fn, Fm;

   floatx80 rFn, rFm;

   int e_flag = opcode & 0x400000;	

   int n_flag = opcode & 0x200000;	

   unsigned int flags = 0;



   



   Fn = getFn(opcode);

   Fm = getFm(opcode);



   

   switch (fpa11->fType[Fn])

   {

      case typeSingle:

        

	if (float32_is_nan(fpa11->fpreg[Fn].fSingle))

	   goto unordered;

        rFn = float32_to_floatx80(fpa11->fpreg[Fn].fSingle, &fpa11->fp_status);

      break;



      case typeDouble:

        

	if (float64_is_nan(fpa11->fpreg[Fn].fDouble))

	   goto unordered;

        rFn = float64_to_floatx80(fpa11->fpreg[Fn].fDouble, &fpa11->fp_status);

      break;



      case typeExtended:

        

	if (floatx80_is_nan(fpa11->fpreg[Fn].fExtended))

	   goto unordered;

        rFn = fpa11->fpreg[Fn].fExtended;

      break;



      default: return 0;

   }



   if (CONSTANT_FM(opcode))

   {

     

     rFm = getExtendedConstant(Fm);

     if (floatx80_is_nan(rFm))

        goto unordered;

   }

   else

   {

     

      switch (fpa11->fType[Fm])

      {

         case typeSingle:

           

	   if (float32_is_nan(fpa11->fpreg[Fm].fSingle))

	      goto unordered;

           rFm = float32_to_floatx80(fpa11->fpreg[Fm].fSingle, &fpa11->fp_status);

         break;



         case typeDouble:

           

	   if (float64_is_nan(fpa11->fpreg[Fm].fDouble))

	      goto unordered;

           rFm = float64_to_floatx80(fpa11->fpreg[Fm].fDouble, &fpa11->fp_status);

         break;



         case typeExtended:

           

	   if (floatx80_is_nan(fpa11->fpreg[Fm].fExtended))

	      goto unordered;

           rFm = fpa11->fpreg[Fm].fExtended;

         break;



         default: return 0;

      }

   }



   if (n_flag)

   {

      rFm.high ^= 0x8000;

   }



   return PerformComparisonOperation(rFn,rFm);



 unordered:

   

   flags |= CC_OVERFLOW;

   flags &= ~(CC_ZERO | CC_NEGATIVE);



   if (BIT_AC & readFPSR()) flags |= CC_CARRY;



   if (e_flag) float_raise(float_flag_invalid, &fpa11->fp_status);



   writeConditionCodes(flags);

   return 1;

}