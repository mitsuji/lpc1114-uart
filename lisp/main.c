// gcc -std=c99 -o lisp main.c

#include <stdlib.h>

#include "io.h"
#include "stacka.h"
#include "cell.h"
#include "lexia.h"


// S式
// symbol, number はまとめてくる
// '(', ')', spase

// cons
// car
// cdr
// atom
// equal

// ひとまず int16_t の四則演算
// add
// sub
// mul
// div
// mod

// symbol, int16_t, cons
// symbol length max -> 8byte (8 or 7)
// symbol length max -> 6byte?

// pointer 2byte
// symbol  length max 6
// cell size max 8byte

// define, set! したもの以外は eval,print後 消してOK
// cons sell をたどって free ?

// 
// * [re] symbol table 
// ** cons
// ** car
// ** cdr
// ** add
// ** sub
// ** mul
// ** div
// ** mod
// * []
//
//
//

// enum { }


// lexia
// * manage buffer
// * generate token
//

// allocator
// * sram   4kb
// * flash 32kb
//
// define したら flashにコピー
// eval したら sram から消す
//

// cons cel
// *
// *
//

// synta
// * 
// * generate object tree
//

// eval



void myexit(int code);


void test_cell();
void test_lexia();


int main ( int argc, char** argv )
{
  test_cell();
  //  test_lexia();

}


void myexit(int code) {
  io_printf("%d bytes left hanging\n", 0);
  exit(code);
}


void test_cell()
{
  char * head = malloc(4096);
  stacka_init(head,4096);
  
  cell_init();
  while(1){
    cell * c, * e;
    cell_read(&c);
    cell_eval(&e,c);
    cell_print(e);
    io_printf("\n");
  }
  
  stacka_free();
  free(head);
}


void test_lexia()
{
  while(1)
    {
      int ecode;
      char * token;
      if ( ecode = lexia_get_token(&token) == 0 )
	{
	  io_printf("%s\n",token);
	}
      else
	{
	  myexit(ecode);
	}
    }
}

