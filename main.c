#include "LPC214x.h"
#include <stdio.h>

#include "serial.h"

void setup (void) {
  system_init(); /* init PLL, MAM etc. */
  IODIR0 |= (1 << 31);
  IOCLR0 |= (1 << 31); //Turn on USB LED

  //Init UART0 for debug
  PINSEL0 |= 0x00000005; //enable uart0
  U0LCR = 0x83; // 8 bits, no Parity, 1 Stop bit, DLAB = 1
  U0DLM = 0x00;
  U0DLL = 0x20; // 115200 Baud Rate @ 58982400 VPB Clock
  U0LCR = 0x03; // DLAB = 0

  //Init rprintf
  rprintf_devopen(putc_serial0);
}

int main (void) {
  unsigned int val, chan;
  char s[21] = "12345678901234567890";

  setup();

  rprintf("DEBUG: can use rprintf\n");

  rprintf("ok> ");
  char c = getc0();
  rprintf("\nhere it is again: %c\n\n", c);

  return 0; /* never reached */
}
