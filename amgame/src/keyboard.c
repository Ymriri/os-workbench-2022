#include "my_utils.h"

/**
#define KEYNAME(key) \
  [AM_KEY_##key] = #key,

static const char *key_names[] = {
    AM_KEYS(KEYNAME)};
*/
int print_key()
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  // 读取io事件
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    switch (event.keycode)
    {
    case 42:
      Print("user input up\n");
      /* code */
      return 1;
      break;
    case 55:
      Print("user input down\n");
      return 2;
      break;
    case 67:
      Print("user input left\n");
      return 3;
      break;
    case 71:
      Print("user input right\n");
      return 4;
      break;
    case 68:
      Print("user input esc\n");
      // exit
      return -1;
      break;
    default:
      return 0;
    }
  }
  return 0;
}
