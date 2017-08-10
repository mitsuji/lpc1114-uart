#include "lexia.h"

#include "io.h"
#include <ctype.h>
#include <string.h>

static char loc_buff[7];
static int loc_buffused;

static int loc_has_unget_char;
static int loc_unget_char;


static int getc ()
{
  if (loc_has_unget_char)
    {
      loc_has_unget_char = 0;
      return loc_unget_char;
    }
  else
    {
      return io_getc();
    }
}

static int ungetc (int c)
{
  loc_unget_char = c;
  loc_has_unget_char = 1;
  return 0;
}



static int append (char ch)
{
  if( loc_buffused < sizeof(loc_buff) -1 )
    {
      loc_buff[loc_buffused++] = ch;
      return 0;
    }
  else
    {
      return LEXIA_BUFF_OVERFLOW;
    }
}

static char * get_str ()
{
  loc_buff[loc_buffused++] = '\0';
  return loc_buff;
}


int lexia_init ()
{
  loc_has_unget_char = 0;
}

int lexia_get_token (char ** out_str)
{
  int ecode;
  int ch;
  char comment=0;

  loc_buffused = 0;
  do
    {
      if((ch = getc()) == IO_EOF)
	{
	  return LEXIA_EOF;
	}
      
      if(ch == ';')
	{
	  comment = 1;
	}
      
      if(ch == '\n')
	{
	  comment = 0;
	}
      
    }
  while(isspace(ch) || comment);


  if (ecode = append(ch) !=0 )
    {
      return ecode;
    }
  
  if(strchr("()\'", ch))
    {
      *out_str = get_str();
      return 0;
    }
  
  for(;;)
    {
      if((ch = getc()) == IO_EOF)
	{
	  return LEXIA_EOF;
	}
      
      if(strchr("()\'", ch) || isspace(ch))
	{
	  ungetc(ch);
	  *out_str = get_str();
	  return 0;
	}
      
      if (ecode = append(ch) !=0 )
	{
	  return ecode;
	}
      
    }  

}

