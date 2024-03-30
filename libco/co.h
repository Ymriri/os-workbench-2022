#ifndef __CO_H__
#define __CO_H__

// 栈大小
#define STACK_SIZE (1 << 14)

struct co *co_start(const char *name, void (*func)(void *), void *arg);
void co_yield ();
void co_wait(struct co *co);

#endif