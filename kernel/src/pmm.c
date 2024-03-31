#include <common.h>

#define MAX_SIZE 1000
// #include <assert.h>
// 内存对齐
size_t mem_alignment(size_t size);

typedef struct pmm_node
{
  size_t begin;           // 区间开始
  size_t node_width;      // 节点宽度
  struct pmm_node *left;  // 左边索引
  struct pmm_node *right; // 右边索引
} PNOde;

int NODE_COUNT = 0;
struct pmm_node NODE_MAP[1000];

// header
struct pmm_node *head, *is_alloc;

// findg block
size_t find_block(PNOde *root, size_t size);
void free_block(size_t begin);
void insert_head(PNOde *root);

void insert_is_alloc(size_t begin, size_t width); // 插入 分配的节点

PNOde *find_is_alloc(size_t begin); // 找到已分配的块，并且释放

static void *kalloc(size_t size)
{
  size_t begin = find_block(head, mem_alignment(size));
  if (!begin)
  {
    return NULL;
  }
  return (void *)begin;
}

static void kfree(void *ptr)
{
  size_t begin = (size_t)ptr;
  // 回归
  free_block(begin);
}

static void pmm_init()
{
  // 设置后的堆大小
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  // 区间树初始化
  head = &NODE_MAP[NODE_COUNT++];

  head->begin = (size_t)heap.start;

  head->node_width = pmsize;
  head->left = head->right = NULL;

  is_alloc = NULL;
}

MODULE_DEF(pmm) = {
    .init = pmm_init,
    .alloc = kalloc,
    .free = kfree,
};

// 内存对齐
size_t mem_alignment(size_t size)
{
  // assert(size > 0);

  int count = 1;
  while ((size = size >> 1) > 0)
  {
    count++;
  }
  return 1 << count;
}

// 找到第一个可用的区块，然后分配
size_t find_block(PNOde *root, size_t size)
{
  // assert(root != NULL);
  if (root->node_width > size)
  {
    size_t begin = root->begin;
    // 记录已经分配
    insert_is_alloc(begin, size);
    root->begin = begin + 1;
    root->node_width = root->node_width - size;
    return begin;
  }
  if (root->right != NULL)
    return find_block(root->right, size);
  return 0;
}

// 保存已经分配的块
void insert_is_alloc(size_t begin, size_t width)
{
  PNOde *temp = &NODE_MAP[NODE_COUNT++];
  temp->begin = begin;
  temp->node_width = width;
  if (is_alloc != NULL)
  {
    is_alloc->left = temp;
  }
  else
  {
    temp->left = NULL;
  }
  temp->right = is_alloc;
  is_alloc = temp;
}

PNOde *find_is_alloc(size_t begin)
{
  // assert(is_alloc != NULL);
  PNOde *temp = is_alloc;
  while (temp != NULL && temp->begin != begin)
  {
    temp = temp->right;
  }
  temp->left = temp->right;
  if (temp->right != NULL)
    temp->right->left = temp->left;
  return temp;
}

void insert_head(PNOde *root)
{
  // assert(root != NULL);
  head->left = root;
  root->right = head;
  head = root;
}

void free_block(size_t begin)
{
  PNOde *freeBlok = find_is_alloc(begin);
  insert_head(freeBlok);
  // 直接丢到可用里面去把
}
