#include <axtls-lite/os_int.h>

#ifdef PIC32MZ

#include <stdarg.h>

#define PAD_RIGHT 1
#define PAD_ZERO 2

void *memcpy(void *dst, const void *src, uint32_t n)
{
  char *r1 = dst;
  const char *r2 = src;

  while (n--)
    *r1++ = *r2++;

  return dst;
}

void *memset(void *s, int32_t c, uint32_t n)
{
  char *p = (char *)s;

  while (n--)
    *p++ = (char)c;

  return s;
}

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p)
{
  uint64_t quot = 0, qbit = 1;
 
  if (den == 0){
    return 1 / ((uint32_t)den);
  }
 
  while ((int64_t)den >= 0){
    den <<= 1;
    qbit <<= 1;
  }
 
  while (qbit){ 
    if (den <= num){
      num -= den;
      quot += qbit;
    }
    den >>= 1;
    qbit >>= 1;
  }
 
  if (rem_p)
    *rem_p = num;
  
  return quot;
}

uint64_t __umoddi3(uint64_t num, uint64_t den)
{
  uint64_t v;
 
  (void) __udivmoddi4(num, den, &v);
  return v;
}

uint64_t __udivdi3(uint64_t num, uint64_t den)
{
  return __udivmoddi4(num, den, NULL);
}

static void printchar(char **str, int c){   
  if (str) {
    **str = c;
    ++(*str);
  }
  else (void)_putchar(c);
}

static int32_t prints(char **out, const char *string, int width, int pad){
  register int pc = 0, padchar = ' ';

  if (width > 0) {
    register int len = 0;
    register const char *ptr;
    for (ptr = string; *ptr; ++ptr) ++len;
    if (len >= width) width = 0;
    else width -= len;
    if (pad & PAD_ZERO) padchar = '0';
  }
  if (!(pad & PAD_RIGHT)) {
    for ( ; width > 0; --width) {
      printchar (out, padchar);
      ++pc;
    }
  }
  for ( ; *string ; ++string) {
    printchar (out, *string);
    ++pc;
  }
  for ( ; width > 0; --width) {
    printchar (out, padchar);
    ++pc;
  }

  return pc;
}

static int32_t printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
  char print_buf[12];
  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;

  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints (out, print_buf, width, pad);
  }

  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  }

  s = print_buf + 11;
  *s = '\0';

  while (u) {
    t = u % b;
    if( t >= 10 )
      t += letbase - '0' - 10;
    *--s = t + '0';
    u /= b;
  }

  if (neg) {
    if( width && (pad & PAD_ZERO) ) {
      printchar (out, '-');
      ++pc;
      --width;
    }
    else {
      *--s = '-';
    }
  }

  return pc + prints (out, s, width, pad);
}


static int32_t print(char **out, const char *format, va_list args )
{
  register int width, pad;
  register int pc = 0;
  char scr[2];

  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = 0;
      if (*format == '\0') break;
      if (*format == '%') goto out;
      if (*format == '-') {
        ++format;
        pad = PAD_RIGHT;
      }
      while (*format == '0') {
        ++format;
        pad |= PAD_ZERO;
      }
      for ( ; *format >= '0' && *format <= '9'; ++format) {
        width *= 10;
        width += *format - '0';
      }
      if( *format == 's' ) {
        register char *s = (char *)va_arg( args, int );
        pc += prints (out, s?s:"(null)", width, pad);
        continue;
      }
      if( *format == 'd' ) {
        pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
        continue;
      }
      if( *format == 'x' ) {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
        continue;
      }
      if( *format == 'X' ) {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
        continue;
      }
      if( *format == 'u' ) {
        pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
        continue;
      }
      if( *format == 'c' ) {
        /* char are converted to int then pushed on the stack */
        scr[0] = (char)va_arg( args, int );
        scr[1] = '\0';
        pc += prints (out, scr, width, pad);
        continue;
      }
    }
    else {
      
      if (*format == '\n')
        printchar(out, '\r');
      
    out:
      printchar (out, *format);
      ++pc;
    }
  }
  if (out) **out = '\0';
  va_end( args );
  return pc;
}

/*Based on Georges Menie Version
contributed by Christian Ettinger*/
int32_t _printf(const char *format, ...)
{
  va_list args;
        
  va_start( args, format );
  
  return print( 0, format, args );
}


#endif /* PIC32MZ */

