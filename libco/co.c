#ifdef LOCAL_MACHINE
#define debug(...) printf(__VA_ARGS__)
#else
#define debug()
#endif

#include "co.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>

// 协程的状态
enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

enum setjmp_status
{
  SJ_SAVE = 0,
  SJ_RECOVERY,
};

typedef struct co
{
  char *name;           // 协程名称
  void *arg;            // 参数名称
  void (*func)(void *); // 指定入口地址

  enum co_status status; // 状态

  struct co *waiter;         // 执行完唤醒谁
  jmp_buf context;           // 寄存器现场 跳跃标记点，保留程序环境
  uint8_t stack[STACK_SIZE]; // 模拟协程栈
  struct co *next;
  struct co *pre;
} Co;

void co_wrapper(void entry(void *), uintptr_t arg);

// 实验自带的 栈 自动保存栈数据
static inline void stack_switch_call(void *sp, void entry(void *), uintptr_t arg)
{
  asm volatile(
#if __x86_64__
      "movq %0, %%rsp"
      :
      : "g"((uintptr_t)sp)
      : "memory"
#else
      "movl %0, %%esp"
      :
      : "g"((uintptr_t)sp)
      : "memory"
#endif
  );
   // 标记该协程已经完成,自动摧毁
  co_wrapper(entry, arg);
}

Co *head = NULL, *tail = NULL, *current = NULL;

static void bl_insert(Co *co)
{
  tail->next = co;
  co->pre = tail;
  co->next = NULL;
  tail = co;
}

// remove co
static void bl_remove(Co *co)
{
  Co *temp_co = co;
  // 保证head非空
  if (temp_co == tail)
  {
    tail = temp_co->pre;
    tail->next = NULL;
  }
  else
  {
    Co *pco = temp_co->pre;
    Co *nco = temp_co->next;
    pco->next = nco;
    nco->pre = pco;
  }
  free(temp_co);
}

// init main

static __attribute__((constructor)) void co_init()
{
  head = (Co *)malloc(sizeof(Co));
  Co *main_co = co_start("main", NULL, NULL);
  // 创建main
  main_co->status = CO_RUNNING;
  main_co->pre = head;
  head->next = main_co;
  // first set current is main
  tail = current = main_co;
  printf("init ok!\n");
}

static __attribute__((destructor)) void co_free()
{
  Co *itr = tail;
  while (itr != head)
  {
    bl_remove(itr);
    itr = itr->pre;
  }
  free(head);
}

// 开始协程, 其实就是丢到协程管理里面
struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  Co *co = (Co *)malloc(sizeof(Co));
  memset(co, 0, sizeof(0));
  co->name = (char *)name;
  co->func = func;
  co->arg = arg;
  co->status = CO_NEW;
  co->next = co->pre = NULL;

  if (head->next != NULL)
    bl_insert(co);
  return co;
}

// co_ wait
void co_wait(struct co *co)
{
  co->waiter = current;
  if (co->status != CO_DEAD)
  {
    // 当前协程等待
    current->status = CO_WAITING;
    co_yield ();
  }
  assert(co->status == CO_DEAD && co->waiter == current);
  // 显然co协程已经执行完毕
  bl_remove(co);
  // 需要释放，每个协程只能被wait一次，除初始协程外，其他协程都必须被co_wait恰好一次
}

// 下一个协程
void co_yield ()
{
  int ret = 0;
  // if return 0, go next. return others return.
  // save context
  if ((ret = setjmp(current->context)) == SJ_RECOVERY)
    return;
  while (true)
  {
    // 自动切换到下一个执行
    current = current->next;
    // 训练链表方式执行
    if (current == NULL)
      current = head->next;
    switch (current->status)
    {
    case CO_NEW:
      current->status = CO_RUNNING;
      // begin run
      stack_switch_call(current->stack + STACK_SIZE, current->func, (uintptr_t)current->arg);
      break;
    case CO_RUNNING:
      longjmp(current->context, SJ_RECOVERY);
      break;
    case CO_DEAD:
      // 唤起它的等待者
      if (current->waiter != NULL)
      {
        current->waiter->status = CO_RUNNING;
      }
      break;
    case CO_WAITING:
      break;
    default:
      assert(false);
    }
  }
}

void co_wrapper(void entry(void *), uintptr_t arg)
{
  entry((void *)arg);
  current->status = CO_DEAD;
  co_yield ();
}
