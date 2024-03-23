#ifndef _GAMW_H_
#define _GAMW_H_

#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "my_utils.h"
// 引入外部函数
void splash();

void changeImg();
void inputImg(int cmd);
int print_key();

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}

#endif
