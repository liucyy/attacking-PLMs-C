unsigned int EmulateAll(unsigned int opcode, FPA11* qfpa, CPUARMState* qregs)

{

  unsigned int nRc = 0;



  FPA11 *fpa11;





  qemufpa=qfpa;

  user_registers=qregs;



#if 0

  fprintf(stderr,"emulating FP insn 0x%08x, PC=0x%08x\n",

          opcode, qregs[REG_PC]);

#endif

  fpa11 = GET_FPA11();



  if (fpa11->initflag == 0)		

  {

    resetFPA11();

    SetRoundingMode(ROUND_TO_NEAREST);

    SetRoundingPrecision(ROUND_EXTENDED);

    fpa11->initflag = 1;

  }



  set_float_exception_flags(0, &fpa11->fp_status);



  if (TEST_OPCODE(opcode,MASK_CPRT))

  {

    

    

    

    

    nRc = EmulateCPRT(opcode);

  }

  else if (TEST_OPCODE(opcode,MASK_CPDO))

  {

    

    

    

    nRc = EmulateCPDO(opcode);

  }

  else if (TEST_OPCODE(opcode,MASK_CPDT))

  {

    

    

    

    nRc = EmulateCPDT(opcode);

  }

  else

  {

    

    nRc = 0;

  }





  if(nRc == 1 && get_float_exception_flags(&fpa11->fp_status))

  {

    

    nRc -= get_float_exception_flags(&fpa11->fp_status);

  }



  

  return(nRc);

}