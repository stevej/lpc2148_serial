#include "LPC214x.h"

#define PLOCK 0x400
/**
 * Much of the bit-fiddling setup code came from futz's RS232 article:
 * http://bit.ly/bbpgj2
 */
void init(void);
void rs232init(void);
void menuout(void);
void p0status(void);
void beep(void);
void flash(void);
char getchar(void);
void putchar(char);
void delay_ms(int);
void flip(int);
extern void reset(void);
extern void system_init(void);

// The pin numbers were found on the UberBoard v2 Schematic
#define RED_LED         (1<<18) // The Red LED is on Port 0-Pin 18
#define GREEN_LED       (1<<19) // The Green LED is on Port 0-Pin 19
#define BLUE_LED        (1<<20) // The Blue LED is on Port 0-Pin 20

static char menu[]={"\r\n\r\n\
SparkFun UberBoard v2 (LPC-2148)\r\n\
----------------------\r\n\
      P - Port0 Status\r\n\
      R - Turn On/Off Red LED\r\n\
      G - Turn On/Off Green LED\r\n\
      B - Turn On/Off Blue LED\r\n\
      L - Flash LEDs\r\n\
      Z - Reset Device\r\n\
   <CR> - Display Menu\r\n"};

int main(void) {
  system_init();
  rs232init();
  // Set the Red, Green and Blue LED pins as outputs
  IODIR0 |= RED_LED | GREEN_LED | BLUE_LED;

  // Initially turn all of the LEDs off
  IOSET0 = RED_LED | GREEN_LED | BLUE_LED;
  // clear the screen
  for (int x = 0; x < 40; x++) {
    putchar(0x0d); putchar(0x0a);
  }

  write_menu();
  while (1) {
    switch (getchar()) {
      // CR
    case 0x0D:
      write_menu();
      break;
    case 'p':
    case 'P':
      p0status();
      break;
    case 'r': // Red LED
    case 'R':
      flip('r');
      break;
    case 'g': // Green LED
    case 'G':
      flip('g');
      break;
    case 'b': // Blue LED
    case 'B':
      flip('b');
      break;
    case 'z':
    case 'Z':
      reset();
      break;
    case 'l':
    case 'L':
      flash();
      break;
    }
  }
}

void p0status(void) {
  int i,j,k,l;
  i = IOPIN0;
  for (l=0; l<4; l++) {
    for (j=0; j<8; j++) {
      k = (i & 0x80000000) >> 31;
      putchar(k + 0x30);
      i = i<<1;
    }
    putchar(0x20);
  }
  putchar(0x0d); putchar(0x0a);
  putchar(0x0d); putchar(0x0a);
}

void write_menu (void) {
  int x = 0;

  while (menu[x]) {
    putchar(menu[x]);
    x++;
  }

  putchar(0x0d); putchar(0x0a);    //output CR/LF
}

void flip (int l) {
  switch (l) {
  case 'r': // flip the Red LED
   IOCLR0 = RED_LED;              // Turn on the Red LED
   IOSET0 = GREEN_LED | BLUE_LED; // Make sure Green and Blue are off
   break;
  case 'g':
   IOCLR0 = GREEN_LED;            // Now turn the Green LED on
   IOSET0 = RED_LED | BLUE_LED;   // and turn off Red and Blue
   break;
  case 'b':
   IOCLR0 = BLUE_LED;             // Now turn on the Blue LED
   IOSET0 = RED_LED | GREEN_LED;  // Make sure Red and Green are off
   break;
  case 'a':
    IOSET0 = RED_LED | GREEN_LED | BLUE_LED; // Turn them all off
   break;
  }
}

void flash (void) {
  for (int i=0; i < 100; i++) {
    switch (i % 3) {
    case 0:
      flip('r');
      break;
    case 1:
      flip('g');
      break;
    case 2:
      flip('b');
      break;
    }

    delay_ms(250);
  }

  // turn all LEDs off
  flip('a');
}


char getchar (void) {
  while(!(U0LSR && 0x01));
  return U0RBR;
}

void putchar (char c) {
  while(!(U0LSR && 0x20));
  U0THR = c;
}

void rs232init (void) {
  PINSEL0 = 0x00000005; // enable TXD0/RXD0 pins - P0.0 & 1
  U0LCR = 0x83;         // 8-N-1, enable DLAB
  U0DLM = 0;            // 115200 baud
  U0DLL = 0x13;
  U0FCR = 0x07;         // enable & clear FIFOs
  U0FDR = 0x00000075;   // set fractional divider
  U0LCR = 0x03;         // disable DLAB
}
