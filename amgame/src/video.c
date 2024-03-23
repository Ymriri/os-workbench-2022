#ifndef _GAMW_VIDEO_
#define _GAMW_VIDEO_

#include <game.h>

#define SIDE 16
static int w, h;

int now_x, now_y;

static void init()
{
  now_x = 5;
  now_y = 5;
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
  inputImg(3);
}

static void draw_tile(int x, int y, int w, int h, uint32_t color)
{
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
      .x = x,
      .y = y,
      .w = w,
      .h = h,
      .sync = 1,
      .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++)
  {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash()
{
  init();
  for (int x = 0; x * SIDE <= w; x++)
  {
    for (int y = 0; y * SIDE <= h; y++)
    {
      // 开始全白
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x000000); // white
      /**
       if ((x & 1) ^ (y & 1))
       {
         draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
       }
      */
    }
  }
}
void changeImg()
{
  for (int x = 0; x * SIDE <= w; x++)
  {
    for (int y = 0; y * SIDE <= h; y++)
    {
      if ((x & 1) ^ (y & 1))
      {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x488ce6); // white
      }
    }
  }
}

/**
 * 把x,y位置画红色，原来位置还原成白色
 */
void inputImg(int cmd)
{
  int x = now_x, y = now_y;
  if (cmd == 3 && now_x > 0)
  {
    x--;
  }
  else if (cmd == 4 && (1 + now_x) * SIDE < w)
  {
    x++;
  }
  else if (cmd == 1 && now_y > 0)
  {
    y--;
  }
  else if (cmd == 2 && (now_y + 1) * SIDE < h)
  {
    y++;
  }
  draw_tile(now_x * SIDE, now_y * SIDE, SIDE, SIDE, 0x000000);
  // draw red
  draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xd75442);
  now_x = x;
  now_y = y;
}
#endif