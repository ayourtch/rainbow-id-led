/* compile on host platform! */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

unsigned long value = 0;

void just_blink_INIT(void) {
}

void just_blink_R(void) {
   printf("*R\n");
}

void just_blink_G(void) {
   printf("*G\n");
}

void just_blink_B(void) {
   printf("*B\n");
}

void just_blink_X(void) {
   printf("----\n");
}

char just_blink_P(void) {
  unsigned long i;
  printf("delay: %lu\n", value);
  return 0;
}

// true if we need to interrupt
char just_blink_INT(void) {
  return 0;
}

static int count = 3;

char just_blink_INT_ITER(void) {
 printf("END LOOP, count: %d\n", count);
 if (--count > 0) {
   return 0;
 } else {
   return 1; // return 1;
 }

}

#include "just_blink_lang.inc"

int main(int argc, char *argv[]) {
   if(argc < 2) {
     printf("Need the just_blink command string as argument\n");
     return 1;
   }
   char *arg_end = argv[1] + strlen(argv[1]);
   just_blink(argv[1], arg_end);
}
