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

#define PSETX do { PORTB |= 0b100; } while (0)
#define PCLRX do { PORTB &= ~0b100; } while (0)

static inline void
bitdelay ()
{
  int i;
  for (i = 0; i < 6; i++)
    {
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
    }
  asm ("nop");
  asm ("nop");
  asm ("nop");
}

static inline void
half_bitdelay ()
{
  int i;
  for (i = 0; i < 3; i++)
    {
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
      asm ("nop");
    }
  asm ("nop");
  asm ("nop");
  asm ("nop");
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
  unsigned int timeout = 65535;
  while ((0 == (PINB & 0b10000)) && (--timeout));	/* spin */
  if (timeout == 0) {
    return 0xff;
  }
   
  half_bitdelay ();
  for (i = 0; i < 8; i++)
    {
      bitdelay ();
      acc = acc >> 1;
      PSETX;
      PCLRX;
      if (PINB & 0b10000)
	{
	  acc |= 0;
	}
      else
	{
	  acc |= 0x80;
	}
    }
  return acc;
}

void
just_blink (char *start, char *end)
{
  while (1)
    {
      int i;
      unsigned int value = 0;
      char *pc = start;
      char is_digit = 0;
      while (pc < end) {
        switch (*pc) {
          case 'R': { PORTB |= 0b1100; break; }
          case 'G': { PORTB |= 0b0001; break; }
          case 'B': { PORTB |= 0b0010; break; }
          case 'X': { PORTB &= ~0b1011; break; }
          case 'P': for (i = 0; i < value; i++) { if ((PINB & 0b10000)) { return; } }; break;
          case 'K': { if (is_digit) { value = 1000*value; }; break; }
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9': 
                    { if (!is_digit) { value = 0; }
                    value = 10*value + (*pc - '0'); 
                    break;
                    }
          case ' ': { break; }
          case ')': { break; }
        }
        if ((PINB & 0b10000)) { return; }
        is_digit = (*pc >= '0') && (*pc <= '9');
        pc++;
      }
      if ((PINB & 0b10000)) { return; }
    }
}

void
just_blink_n (int n)
{
  while (1)
    {
      int i;
      PORTB = 0b1100;
      for (i = 0; i < 10000; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0010;
      for (i = 0; i < 10000; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB = 0b0001;
      for (i = 0; i < 10000; i++) { if ((PINB & 0b10000)) { return; } }
      PORTB &= ~0b1111;
      for (i = 0; i < 10000; i++) { if ((PINB & 0b10000)) { return; } }
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


int
main (void)
{

  char in_buf[200];

  DDRB = 0b01111;
  PCLR;
  just_blink_n(10);

  // just_blink_fast();
  // repeater();

  char *curr_expect = lead_string;
  char *pc = curr_expect;
  char *pcin;
  char *in_buf_eod = in_buf;
  char *pcin_end = in_buf+sizeof(in_buf)-1;
  while (1)
    {
      pcin = in_buf;
      // memset(in_buf, 0, sizeof(in_buf));
      just_blink (in_buf, in_buf_eod);
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
	      pc = curr_expect;
	    }
	}
    }

  while (1)
    {
      sendbyte (0xff);
      sendbyte (0x00);
      sendbyte (0xaa);
      sendbyte (0xaa);
    }

}
