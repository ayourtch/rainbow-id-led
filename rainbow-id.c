/* blink.c
* Created: 23/09/2013 21:04:02
* Author: G.J. van Loo
* Simple example program to 'walk' the LEDs */

#include <avr/io.h>
#include <avr/sleep.h>
#include <string.h>

#define F_CPU 16000000
// Some macros that make the code more readable 
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir&= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

// static int is_running = 0;

// Outputs are:

#define PSET do { PORTB |= 0b01000; } while (0)
#define PCLR do { PORTB &= ~0b01000; } while (0)

#define PSETLED do { PORTB |= 0b01010; } while (0)
#define PCLRLED do { PORTB &= ~0b01010; } while (0)

#define PSETLEDG do { PORTB |= 0b00001; } while (0)
#define PCLRLEDG do { PORTB &= ~0b00001; } while (0)

#define PSETX do { PORTB |= 0b1000; } while (0)
#define PCLRX do { PORTB &= ~0b1000; } while (0)

// 8113 #define BITDELAY_APPROX 152

// 13179 #define BITDELAY_APPROX 145
// 8213 #define BITDELAY_APPROX 150

// 8324 #define BITDELAY_APPROX 148
// 8447 #define BITDELAY_APPROX 146
// 8686 #define BITDELAY_APPROX 142
// 8918 #define BITDELAY_APPROX 138
// 9248 #define BITDELAY_APPROX 133
// 9512 #define BITDELAY_APPROX 129

//sending values
//#define BITDELAY_APPROX 128
//#define HALFBITDELAY_APPROX  64
#define BITDELAY_APPROX 128
#define HALFBITDELAY_APPROX  60

static int curr_bitdelay = 100;
static int curr_halfbitdelay = 100;
static inline void
bitdelay ()
{
  long int i;
  long int cnt0 = 0;
  long int cnt1 = 0;
  for (i = 0; i < curr_bitdelay; i++)
    {
      if (PINB & 0b10000) {
        cnt1++;
      } else {
        cnt0++;
      }
      asm ("nop");
    }
}

static inline void
half_bitdelay ()
{
  long int i;
  long int cnt0 = 0;
  long int cnt1 = 0;
  for (i = 0; i < curr_halfbitdelay; i++)
    {
      if (PINB & 0b10000) {
        cnt1++;
      } else {
        cnt0++;
      }
      asm ("nop");
    }
}

void
sendbyte (unsigned char c)
{
  char i;
  PCLR;
  bitdelay ();
  for (i = 0; i < 8; i++)
    {
      if (c & 1)
	{
	  PSET;
	}
      else
	{
	  PCLR;
	}
      bitdelay ();
      c = c >> 1;
    }
  PSET;
  bitdelay ();
  bitdelay ();
  bitdelay ();
}

void
sendbyte_inv (unsigned char c)
{
  char i;
  PSET;
  bitdelay ();
  for (i = 0; i < 8; i++)
    {
      if (c & 1)
	{
	  PCLR;
	}
      else
	{
	  PSET;
	}
      bitdelay ();
      c = c >> 1;
    }
  PCLR;
  bitdelay ();
  bitdelay ();
  bitdelay ();
}


unsigned char
recvbyte_norm ()
{
  unsigned char acc = 0;
  char i;
  while (PINB & 0b10000);	/* spin */
  half_bitdelay ();
  for (i = 0; i < 8; i++)
    {
      bitdelay ();
      acc = acc >> 1;
      PSETX;
      PCLRX;
      if (PINB & 0b10000)
	{
	  acc |= 0x80;
	}
      else
	{
	  acc |= 0;
	}
    }
  return acc;
}

unsigned char
recvbyte_inv ()
{
  unsigned char acc = 0;
  char i;
  unsigned long int timeout = 655350;
  while ((0 == (PINB & 0b10000)) && (--timeout));	/* spin */
  if (timeout == 0) {
    return 0xff;
  }
   
  half_bitdelay ();
  for (i = 0; i < 8; i++)
    {
      bitdelay ();
      acc = acc >> 1;
      if (PINB & 0b10000)
	{
      // PSETX;
      PCLRX;
	  acc |= 0;
	}
      else
	{
      // PCLRX;
      PSETX;
	  acc |= 0x80;
	}
    }
  PSETX;
  bitdelay ();
  return acc;
}

unsigned long value = 0;

void just_blink_R(void) {
   PORTB |= 0b1100;
}

void just_blink_G(void) {
   PORTB |= 0b0001;
}

void just_blink_B(void) {
   PORTB |= 0b0010;
}

void just_blink_X(void) {
   PORTB &= ~0b1111;
}

char just_blink_P(void) {
  unsigned long i;
  for (i = 0; i < curr_bitdelay*value/10; i++) { if ((PINB & 0b10000)) { return 1; } };
  return 0;
}

// true if we need to interrupt
char just_blink_INT(void) {
  return (PINB & 0b10000);
}

#include "just_blink_lang.inc"


void
just_blink_n (int n)
{
  while (1)
    {
      long int i;
      PORTB = 0b1100;
      for (i = 0; i < 12800; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0;
      for (i = 0; i < 128000; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0010;
      for (i = 0; i < 12800; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0;
      for (i = 0; i < 128000; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0001;
      for (i = 0; i < 12800; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB &= ~0b1111;
      for (i = 0; i < 128000; i++) { if ((PINB & 0b10000)) { return; } }
      if (n-- == 0) { return; };
    }
}

void
just_blink_fast ()
{
  while (1)
    {
      int i;
      PCLR;
      bitdelay ();
      PSET;
      bitdelay ();
    }
}

void
repeater ()
{
  while (1)
    {
      while (PINB & 0b10000);	/* spin */
      PCLR;
      while (!(PINB & 0b10000));	/* spin */
      PSET;
    }
}


#define recvbyte recvbyte_inv

char *lead_string = "(ILED:";

void test_send() {
  while (1)
    {
      sendbyte (0xff);
      bitdelay(); bitdelay(); bitdelay(); bitdelay(); bitdelay();
      sendbyte (0x00);
      bitdelay(); bitdelay(); bitdelay(); bitdelay(); bitdelay();
      sendbyte (0xaa);
      bitdelay(); bitdelay(); bitdelay(); bitdelay(); bitdelay();
      sendbyte (0xaa);
      bitdelay(); bitdelay(); bitdelay(); bitdelay(); bitdelay();
    }
}

int
main (void)
{

  char in_buf[200];

  DDRB = 0b01111;
  PCLR;
  just_blink_n(3);
  curr_bitdelay = BITDELAY_APPROX;
  curr_halfbitdelay = HALFBITDELAY_APPROX;
  // test_send();

  // just_blink_fast();
  // repeater();

  char *curr_expect = lead_string;
  char *pc = curr_expect;
  char *pcin;
  char *in_buf_eod = in_buf;
  char *pcin_end = in_buf+sizeof(in_buf)-1;
/*

#define STR "XRGB2KPX2KP"
  memcpy(in_buf, STR, strlen(STR)+1);
  in_buf_eod = in_buf + strlen(STR);

*/


  while (1)
    {
      pcin = in_buf;
      // memset(in_buf, 0, sizeof(in_buf));
      just_blink (in_buf, in_buf_eod);
      DDRB &= ~0x7;
      PORTB &= ~0xf;
      while (1)
	{
	  unsigned char acc = recvbyte ();
	  if (acc == *pc)
	    {
	      if (*(++pc) == 0)
		{ /* now capture the string */
                   pc = in_buf;
                   while((pc < pcin_end) && (acc != 0xff)) {
                     *pc++ = acc = recvbyte();
                   }
                   in_buf_eod = pc;
                   pc = curr_expect;
                   break;
		}
	    }
	  else
	    {
              if (acc == 0xff) {
                break;
              }
	      PSETX;
	      PCLRX;
	      PSETX;
	      PCLRX;
	      PSETX;
	      PCLRX;
	      PSETX;
	      pc = curr_expect;
	    }
	}
    }


}
