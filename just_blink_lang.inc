
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
      while (pc < end) {
        was_digit = is_digit;
        is_digit = (*pc >= '0') && (*pc <= '9');
        if ((!is_digit && *pc != 'K') && in_digit) {
          in_digit = 0;
        }
        switch (*pc) {
          case 'R': { just_blink_R(); break; }
          case 'G': { just_blink_G(); break; }
          case 'B': { just_blink_B(); break; }
          case 'X': { just_blink_X(); break; }
          case '$': { value = get_arg(value); }
          case '>': { push_arg(value); }
          case '<': { pop_arg(); }
          case 'P': { if (just_blink_P()) { return; } break; }
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
