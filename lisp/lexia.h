#ifndef __LISP_LEXIA_H_
#define __LISP_LEXIA_H_

#define LEXIA_BUFF_OVERFLOW 0x00020001
#define LEXIA_EOF           0x00020002

int lexia_init ();
int lexia_get_token (char ** out_str);


#endif
