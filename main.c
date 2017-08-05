#include <stdint.h>
#include "LPC1100.h"

void UART_config(void);
char UART_putc(char ch);
char UART_getc(void);
int  uart_status_rxd(void);
int  uart_status_txd(void);


int main()
{
  UART_config();
  
  UART_putc('H');
  UART_putc('e');
  UART_putc('l');
  UART_putc('l');
  UART_putc('o');
  UART_putc(',');
  UART_putc('U');
  UART_putc('A');
  UART_putc('R');
  UART_putc('T');
  UART_putc('!');
  UART_putc('\n');
  
  for(;;){
    char c = UART_getc();
    UART_putc(c);
  }
}


void UART_config(void)
{
  
  SYSAHBCLKCTRL |= ((1<<16));

  IOCON_PIO1_6 =((0<<5) | (0<<3) | (0x1));
  IOCON_PIO1_7 =((0<<5) | (0<<3) | (0x1));
  
  SYSAHBCLKCTRL |= ((1<<12));
  UARTCLKDIV = 1;
  
  U0LCR = (1<<7) | 3;

  U0DLM = 0;

  // Baud Rate 38400, System Clock 12MHz
  // PCLK / Baud Rate / 16 ≒ (256 * U0DLM + U0DLL) * (1 + DivAddVal / MulVal) 
  // 12,000,000 / 38,400 / 16 ≒ (256 * 0 + 17) * (1 + 1 / 6) 
  //  U0DLL = 67; // 48MHz
  //  U0DLL = 56; // 40MHz
  U0DLL = 17; // 12MHz

  U0FDR = (6<<4) | (1<<0);
  
  U0FCR = (0<<6) | (0<<2)| (0<<1)| 1;
  
  U0LCR = 3;
  
  uart_status_rxd();

  U0IER = 0;

  while (((U0LSR >> 6) & 0x1) == 0);
  
  while (uart_status_rxd() != 0) UART_getc();

  SYSAHBCLKCTRL &= ~((1<<16));

  return;
}

char UART_putc(char ch)
{
  while (uart_status_txd()==0);
  U0THR = (uint32_t)ch;
  return ch;
}

char UART_getc(void)
{
  while (uart_status_rxd()==0);
  return U0RBR;
}


int  uart_status_rxd(void)
{
  return (U0LSR & 0x1);
}

int  uart_status_txd(void)
{
  return ((U0LSR >> 5) & 0x1);
}


void trap (void)
{
  for (;;) ;      /* Trap spurious interrupt */
}

const void* const vector[] __attribute__ ((section(".VECTOR"))) =
/* Vector table to be allocated to address 0 */
{
	(void*)0x10001000,
	main,			/* Reset entry */
	trap,//NMI_Handler,
	trap,//HardFault_Hander,
	0, 0, 0, 0, 0, 0, 0,//<Reserved>
	trap,//SVC_Handler,
	0, 0,//<Reserved>
	trap,//PendSV_Handler,
	trap,
	trap,//PIO0_0_IRQHandler,
	trap,//PIO0_1_IRQHandler,
	trap,//PIO0_2_IRQHandler,
	trap,//PIO0_3_IRQHandler,
	trap,//PIO0_4_IRQHandler,
	trap,//PIO0_5_IRQHandler,
	trap,//PIO0_6_IRQHandler,
	trap,//PIO0_7_IRQHandler,
	trap,//PIO0_8_IRQHandler,
	trap,//PIO0_9_IRQHandler,
	trap,//PIO0_10_IRQHandler,
	trap,//PIO0_11_IRQHandler,
	trap,//PIO1_0_IRQHandler,
	trap,//C_CAN_IRQHandler,
	trap,//SPI1_IRQHandler,
	trap,//I2C_IRQHandler,
	trap,//CT16B0_IRQHandler,
	trap,//CT16B1_IRQHandler,
	trap,//CT32B0_IRQHandler,
	trap,//CT32B1_IRQHandler,
	trap,//SPI0_IRQHandler,
	trap,//UART_IRQHandler,
	0, 0,//<Reserved>
	trap,//ADC_IRQHandler,
	trap,//WDT_IRQHandler,
	trap,//BOD_IRQHandler,
	0,//<Reserved>
	trap,//PIO_3_IRQHandler,
	trap,//PIO_2_IRQHandler,
	trap,//PIO_1_IRQHandler,
	trap //PIO_0_IRQHandler
};

