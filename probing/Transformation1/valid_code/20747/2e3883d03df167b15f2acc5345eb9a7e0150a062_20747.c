get_field (const unsigned char *data, enum floatformat_byteorders order,

           unsigned int total_len, unsigned int start, unsigned int len)

{

  unsigned long result;

  unsigned int cur_byte;

  int cur_bitshift;



  

  cur_byte = (start + len) / FLOATFORMAT_CHAR_BIT;

  if (order == floatformat_little)

    cur_byte = (total_len / FLOATFORMAT_CHAR_BIT) - cur_byte - 1;

  cur_bitshift =

    ((start + len) % FLOATFORMAT_CHAR_BIT) - FLOATFORMAT_CHAR_BIT;

  result = *(data + cur_byte) >> (-cur_bitshift);

  cur_bitshift += FLOATFORMAT_CHAR_BIT;

  if (order == floatformat_little)

    ++cur_byte;

  else

    --cur_byte;



  

  while ((unsigned int) cur_bitshift < len)

    {

      if (len - cur_bitshift < FLOATFORMAT_CHAR_BIT)

	

	result |=

	  (*(data + cur_byte) & ((1 << (len - cur_bitshift)) - 1))

	    << cur_bitshift;

      else

	result |= *(data + cur_byte) << cur_bitshift;

      cur_bitshift += FLOATFORMAT_CHAR_BIT;

      if (order == floatformat_little)

	++cur_byte;

      else

	--cur_byte;

    }

  return result;

}