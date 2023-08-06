match_insn_m68k (bfd_vma memaddr,

		 disassemble_info * info,

		 const struct m68k_opcode * best,

		 struct private * priv)

{

  unsigned char *save_p;

  unsigned char *p;

  const char *d;



  bfd_byte *buffer = priv->the_buffer;

  fprintf_ftype save_printer = info->fprintf_func;

  void (* save_print_address) (bfd_vma, struct disassemble_info *)

    = info->print_address_func;



  

  p = buffer + 2;



  

  for (d = best->args; *d; d += 2)

    {

      

      if (d[0] == '#')

	{

	  if (d[1] == 'l' && p - buffer < 6)

	    p = buffer + 6;

	  else if (p - buffer < 4 && d[1] != 'C' && d[1] != '8')

	    p = buffer + 4;

	}



      if ((d[0] == 'L' || d[0] == 'l') && d[1] == 'w' && p - buffer < 4)

	p = buffer + 4;



      switch (d[1])

	{

	case '1':

	case '2':

	case '3':

	case '7':

	case '8':

	case '9':

	case 'i':

	  if (p - buffer < 4)

	    p = buffer + 4;

	  break;

	case '4':

	case '5':

	case '6':

	  if (p - buffer < 6)

	    p = buffer + 6;

	  break;

	default:

	  break;

	}

    }



  

  if (p - buffer < 4 && (best->match & 0xFFFF) != 0)

    p = buffer + 4;



  

  if (p - buffer < 6

      && (best->match & 0xffff) == 0xffff

      && best->args[0] == '#'

      && best->args[1] == 'w')

    {

      

      p = buffer + 6;

      FETCH_DATA (info, p);

      buffer[2] = buffer[4];

      buffer[3] = buffer[5];

    }



  FETCH_DATA (info, p);



  d = best->args;



  save_p = p;

  info->print_address_func = dummy_print_address;

  info->fprintf_func = (fprintf_ftype) dummy_printer;



  

  for (; *d; d += 2)

    {

      int eaten = print_insn_arg (d, buffer, p, memaddr + (p - buffer), info);



      if (eaten >= 0)

	p += eaten;

      else if (eaten == -1)

	{

	  info->fprintf_func = save_printer;

	  info->print_address_func = save_print_address;

	  return 0;

	}

      else

	{

	  info->fprintf_func (info->stream,

			      

			      _("<internal error in opcode table: %s %s>\n"),

			      best->name,  best->args);

	  info->fprintf_func = save_printer;

	  info->print_address_func = save_print_address;

	  return 2;

	}

    }



  p = save_p;

  info->fprintf_func = save_printer;

  info->print_address_func = save_print_address;



  d = best->args;



  info->fprintf_func (info->stream, "%s", best->name);



  if (*d)

    info->fprintf_func (info->stream, " ");



  while (*d)

    {

      p += print_insn_arg (d, buffer, p, memaddr + (p - buffer), info);

      d += 2;



      if (*d && *(d - 2) != 'I' && *d != 'k')

	info->fprintf_func (info->stream, ",");

    }



  return p - buffer;

}