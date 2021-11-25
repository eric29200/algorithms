#include <math.h>

#include "math.h"

double ieee754_binary64_decode(const char *val)
{
  unsigned short exponent;
  unsigned long fraction;
  double frac;
  char sign;

  /* get sign */
  sign = val[0] & 0x80;

  /* get exponent */
  exponent = ((val[0] << 4) & 0x7F0) | ((val[1] >> 4) & 0x0F);

  /* get fraction */
  fraction = ((unsigned long) (val[1] & 0x0F ) << 48)
    | ((unsigned long) (val[2] & 0xFF ) << 40)
    | ((unsigned long) (val[3] & 0xFF ) << 32)
    | ((unsigned long) (val[4] & 0xFF ) << 24)
    | ((unsigned long) (val[5] & 0xFF ) << 16)
    | ((unsigned long) (val[6] & 0xFF ) <<  8)
    | ((unsigned long) (val[7] & 0xFF))
    | ((unsigned long) 1 << 52);

  /* compute fraction */
  frac = (double) fraction / ((unsigned long) 2 << 52);

  /* infinity and Nan */
  if(exponent == 0x7FF) {
    if(fraction == (unsigned long) 1 << 52)
      return sign ? -1.0 / 0.0 : 1.0 / 0.0;
    else
      return sign ? 0.0 / 0.0 : -(0.0 / 0.0);
  }

  /* compute double */
  return ldexp(frac, exponent - 1022 ) * (sign ? -1 : 1);
}
