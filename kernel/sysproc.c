#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 start_addr;
  uint64 addr;
  int page_count;
  uint64 mask_addr;
  uint64 buffer;
  pte_t *entry;
  struct proc *p;

  argaddr(0, &start_addr);
  argint(1, &page_count);
  argaddr(2, &mask_addr);

  if (page_count > 64) {
    return -1;
  }

  buffer = 0;
  addr = start_addr;
  p = myproc();

  for (int i = 0; i < page_count; i++) {
    entry = walk(p->pagetable, addr, 0);
    if ((*entry & PTE_A) != 0) {

      //  save entry in mask
      buffer = (buffer | (1L << i)); 

      //  set access bit to 0
      *entry = *entry & (~PTE_A);  
    }
    addr = addr + PGSIZE;
  }

  copyout(p->pagetable, mask_addr, (char *)&buffer, 8);

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
