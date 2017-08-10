#include "io.h"
#include <stdio.h>
#include <stdarg.h>

static int io_putc (int c)
{
  return putc(c,stdout);
}


int io_getc ()
{
  return getc(stdin);
}

int io_printf (const char * fmt, ...)
{
  char buff[80];
  va_list args;
  va_start(args, fmt);
  vsprintf(buff, fmt, args);
  va_end(args);

  for (char * p = buff; *p != '\0'; ++p)
    {
      io_putc(*p);
    }
  
  return 0;
}


