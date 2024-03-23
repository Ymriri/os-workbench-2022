#include <game.h>

static inline int uptime_ms()
{
  return io_read(AM_TIMER_UPTIME).us / 1000;
}
static int next_frame;

void wait_for_frame()
{
  int cur = uptime_ms();
  while (cur < next_frame)
  {
    cur = uptime_ms();
  }
  next_frame = cur;
}
#define FPS 60

// Operating system is a C program!
int main(const char *args)
{
  // io init
  // puts("code init!");
  ioe_init();

  puts("welcome ! \"");
  puts(args); // make run mainargs=xxx

  splash();
  next_frame = 0;
  int key = 1;
  while (1)
  {
    wait_for_frame();
    // 一直查询键盘事件
    key = print_key();
    switch (key)
    {
    case 1:
    case 2:
    case 3:
    case 4:
      Print("input%d\n", key);
      inputImg(key);
      break;
    case -1:
      Print("Game over!");
      return 0;
    default:
      break;
    }
    next_frame += 1000 / FPS;
  }
  return 0;
}
