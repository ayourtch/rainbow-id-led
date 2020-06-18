
#define ARG_STACK_DEPTH 16
/* stack of arguments for subroutines, grows from 0 up to ARG_STACK_DEPTH */
static unsigned long arg_stack[ARG_STACK_DEPTH] = { };
static uint8_t arg_stack_first_free = 0;

void push_arg(unsigned long arg)
{
  if (arg_stack_first_free < ARG_STACK_DEPTH) {
     arg_stack[arg_stack_first_free++] = arg;
  }
}

unsigned long pop_arg()
{
  return arg_stack_first_free > 0 ? arg_stack[--arg_stack_first_free] : 0;
}

unsigned long get_arg(uint8_t n)
{
  return arg_stack_first_free > n ? arg_stack[arg_stack_first_free-1-n] : 0;
}

void set_arg(uint8_t n, unsigned long value)
{
  if (n < arg_stack_first_free) {
    arg_stack[arg_stack_first_free-1-n] = value;
  }
}



void
just_blink (char *start, char *end)
{
  just_blink_INIT();
  while (1)
    {
      char *pc = start;
      char is_digit = 0;
      char was_digit = 0;
      char in_digit = 0;
      char in_scan = 0;
      arg_stack_first_free = 0;
      while (pc < end) {
        if (in_scan && (*pc != '!')) {
          pc++;
          continue;
        }
        was_digit = is_digit;
        is_digit = (*pc >= '0') && (*pc <= '9');
        if ((!is_digit && *pc != 'K') && in_digit) {
          push_arg(value);
          in_digit = 0;
        }
        switch (*pc) {
          case 'R': { just_blink_R(); break; }
          case 'G': { just_blink_G(); break; }
          case 'B': { just_blink_B(); break; }
          case 'X': { just_blink_X(); break; }

          /* define/end the subroutine */
          case '!': {
                      if (in_scan) {
                        if (get_arg(0) == pc-start) {
                          /* !! means end of definitions, start running */
                          in_scan = 0;
                          pop_arg();
                        } else {
                          /* define the next subroutine and keep going */
                          push_arg(1+pc-start);
                        }
                      } else {
                        if (arg_stack_first_free > 0) {
                          /* if there is stuff on stack, this means return */
                          unsigned long rv = pop_arg();
                          pc = start + rv;
                          continue;
                        } else {
                          /* running and nothing on stack - jump into definition mode */
                          /* the subroutine code starts from the next symbol */
                          push_arg(1+pc-start);
                          in_scan = 1;
                        }
                      }
                      break;
                    }
          /* return if value is 0 */

          /* push the current address and keep going */
          case '{': { push_arg(pc-start); break; }
          /* pop top, pop the address and go to it if the top value is not zero */
          case '}': { if (pop_arg() != 0) { pc = start + pop_arg(); continue; } else { pop_arg(); } break; }

          /* arithmetic and logical ops */
          case '-': { value = pop_arg(); value = pop_arg() - value; push_arg(value); break; }
          case '+': { value = pop_arg(); value = pop_arg() + value; push_arg(value); break; }
          case '*': { value = pop_arg(); value = pop_arg() * value; push_arg(value); break; }
          /* avoid the uncomfortable division by zero */
          case '/': { value = pop_arg(); value = pop_arg() / (value == 0 ? 1 : value); push_arg(value); break; }
          case '%': { value = pop_arg(); value = pop_arg() % (value == 0 ? 1 : value); push_arg(value); break; }
          case '|': { value = pop_arg(); value = pop_arg() | value; push_arg(value); break; }
          case '&': { value = pop_arg(); value = pop_arg() & value; push_arg(value); break; }
          case '^': { value = pop_arg(); value = pop_arg() ^ value; push_arg(value); break; }
          case '~': { value = pop_arg(); push_arg(~value); break; }

          /* call the subroutine */
          case 'C': { value = pop_arg(); push_arg(pc-start+1); pc = arg_stack[value] + start; continue; break; }

          /* explicit stack operations */
          case ',': { push_arg(value); break; }
          case '.': { value = pop_arg(); break; }

          /* direct load. To make it indirect, just use twice */
          case '$': { value = pop_arg(); push_arg(get_arg(value)); break; }
          /* indirect store - pop the top value and store it into value-th arg */
          case '@': { value = pop_arg(); set_arg(value, pop_arg()); break; }

          case 'P': { value = pop_arg(); if (just_blink_P()) { return; } break; }
/*
          case 'S': { int i; printf("STACK(at %d): ", pc-start);
                      for (i=0; i<arg_stack_first_free; i++) { printf(" %lu", arg_stack[i]); }
                      printf("\n"); break; }
*/
          case 'K': { if (was_digit) { value = 1000*value; }; break; }
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9': 
                    { if (!was_digit) { value = 0; }
                    value = 10*value + (*pc - '0'); 
                    in_digit = 1;
                    break;
                    }
          case ' ': { break; }
          case ')': { break; }
        }
        if (just_blink_INT()) { return; }
        pc++;
      }
      if (just_blink_INT_ITER()) { return; }
    }
}
