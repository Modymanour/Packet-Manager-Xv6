#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
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
  if(n < 0)
    n = 0;
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



#define PKGDB_MAX 100
#define PKGNAME_LEN 15

static char pkgdb[PKGDB_MAX][PKGNAME_LEN+1];
static int pkgdb_count = 0;

static int
str_compare(const char *s1, const char *s2, int n)
{
  for (int i = 0; i < n; i++) {
    if (s1[i] != s2[i])
      return (unsigned char)s1[i] - (unsigned char)s2[i];
    if (s1[i] == '\0')
      return 0;
  }
  return 0;
}

static void
str_copy(char *dst, const char *src, int n)
{
  int i;
  for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
}

uint64
sys_pkgdb_op(void)
{
  int op;
  char buf[PKGNAME_LEN + 1];

  argint(0, &op);
  argstr(1, buf, sizeof(buf));

  int found = 0;
  int i;

  if (op == 1) {  // install
    for (i = 0; i < pkgdb_count; i++) {
      if (str_compare(pkgdb[i], buf, PKGNAME_LEN) == 0) {
        found = 1;
        break;
      }
    }
    if (found)
      return 0; // already installed

    if (pkgdb_count >= PKGDB_MAX)
      return -1; // db full

    str_copy(pkgdb[pkgdb_count], buf, PKGNAME_LEN + 1);
    pkgdb_count++;
    return 0;

  } else if (op == 2) { // remove
    for (i = 0; i < pkgdb_count; i++) {
      if (str_compare(pkgdb[i], buf, PKGNAME_LEN) == 0) {
        found = 1;
        for (; i < pkgdb_count - 1; i++) {
          str_copy(pkgdb[i], pkgdb[i + 1], PKGNAME_LEN + 1);
        }
        pkgdb_count--;
        return 0;
      }
    }
    return -1; // not found

  } else if (op == 3) { // update
    // For update, check if package exists
    for (i = 0; i < pkgdb_count; i++) {
      if (str_compare(pkgdb[i], buf, PKGNAME_LEN) == 0) {
        // Simulate update success (you can add real logic)
        return 0;
      }
    }
    return -1; // package not found to update
  }

  return -1; // unknown op
}
