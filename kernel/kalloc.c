// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

typedef struct kmem {
  struct spinlock lock;
  struct run *freelist;
} kmem_t;

static kmem_t kmem[NCPU];

void
kinit()
{
  for (int i = 0; i < NCPU; i++) {
    initlock(&kmem[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}


void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  kfree(p);
}

// move a part of a process freelist to another cpu
// call only with both cpu locks acquired
// only call with 2 different cpu's
void
steal(int taking, int giving)
{
  struct run *head;
  struct run *tail;
  uint count;

  count = 0;

  head = kmem[giving].freelist;
  while (head != 0) {
    count++;
    head = head->next;
  }

  count = (count / 2);
  head = kmem[giving].freelist;

  tail = head;
  for (int i = 0; i < count; i++) {
    tail = tail->next;
    if (tail->next == 0) {
      break;
    } 
  }
  kmem[giving].freelist = tail->next;

  // add the pages to the taking cpu
  tail->next = kmem[taking].freelist;
  kmem[taking].freelist = head;
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  int cpu;
  struct run *r;
  
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
  panic("kfree");
  
  push_off();
  cpu = cpuid();
  
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
  
  r = (struct run*)pa;
  
  acquire(&kmem[cpu].lock);
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;
  release(&kmem[cpu].lock);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  int cpu;
  struct run *r;
  int i;
  
  push_off();
  cpu = cpuid();
  
  acquire(&kmem[cpu].lock);
  if(kmem[cpu].freelist == 0) {
    release(&kmem[cpu].lock);
    
    for (i = 0; i < NCPU ; i++) {
      if (i != cpu) {
        acquire(&kmem[i].lock);
        if (kmem[i].freelist == 0) {
          release(&kmem[i].lock);
          continue;
        }
        acquire(&kmem[cpu].lock);
        steal(cpu, i);
        if (kmem[cpu].freelist != 0) {
          release(&kmem[i].lock);
          break;
        }
        release(&kmem[i].lock);
        release(&kmem[cpu].lock);
      }
    }
    if (i == NCPU) {
      pop_off();
      return 0;
    }
  }
  
  r = kmem[cpu].freelist;
  if(r)
  kmem[cpu].freelist = r->next;
  release(&kmem[cpu].lock);
  pop_off();
  
  if(r)
  memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}




void
printlists(void) {
  struct run *r;
  int length;
  printf("-----   CPU FREELISTS   -----\n");
  for (int i = 0; i < NCPU; i++) {
    acquire(&kmem[i].lock);
    r = kmem[i].freelist;
    length = 0;
    while (r != 0) {
      r = r->next;
      length++;
    }
    printf("core %d: %d\n", i, length);
    release(&kmem[i].lock);
  }
}