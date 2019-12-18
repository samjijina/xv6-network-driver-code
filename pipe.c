#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
  struct selproc selprocread;
  struct selproc selprocwrite;
};

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *p;

  p = 0;
  *f0 = *f1 = 0;
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  if((p = (struct pipe*)kalloc()) == 0)
    goto bad;
  p->readopen = 1;
  p->writeopen = 1;
  p->nwrite = 0;
  p->nread = 0;
  
  initselproc(&p->selprocread);
  initselproc(&p->selprocwrite);
 
  initlock(&p->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;
  return 0;

//PAGEBREAK: 20
 bad:
  if(p)
    kfree((char*)p);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}

void
pipeclose(struct pipe *p, int writable)
{
  acquire(&p->lock);
  if(writable){
    p->writeopen = 0;
    // Wake up anything waiting to read
    // Lab 4: Your code here.
    
    wakeup(&p->nread);
  } else {
    p->readopen = 0;
    // Wake up anything waiting to write
    // LAB 4: Your code here
    
    wakeup(&p->nwrite);
  }
  if(p->readopen == 0 && p->writeopen == 0){
    release(&p->lock);
    kfree((char*)p);
  } else
    release(&p->lock);
}

//PAGEBREAK: 40
int
pipewrite(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || myproc()->killed){
        release(&p->lock);
        return -1;
      }
      wakeupselect(&p->selprocread);
      wakeup(&p->nread);
      sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
  }
  
  // Wake up anything waiting to read
  // LAB 4: Your code here
  
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}

int
piperead(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
    if(myproc()->killed){
      release(&p->lock);
      return -1;
    }
    sleep(&p->nread, &p->lock); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    addr[i] = p->data[p->nread++ % PIPESIZE];
  }
  
  // Wake up anything waiting to write
  // LAB 4: Your code here
  
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}

/* Checks if this pipe is writeable or not.
 *
 * Requirements:
 *
 * 1. Return -1 if the pipe is not open for reading or the process has been killed.
 * 2. Check if the pipe is writeable and return 1 if yes and 0 if not.
 */
int
pipewriteable(struct pipe *p)
{
    // LAB 4: Your code here
    return 0;
}

/* Checks if this pipe is readable or not.
 *
 * Requirements:
 *
 * 1. If the process has been killed, return -1.
 * 2. If the pipe is non-empty or closed, return 1; otherwise 0.
 */
int
pipereadable(struct pipe *p)
{
    // LAB 4: Your code here
    return 0;
}

/* Sets a wakeup call.
 *
 * Requirements:
 *
 * 1. Use addselid to add the selid channel to the list of wakeups
 *
 */
int
pipeselect(struct pipe *p, int * selid, struct spinlock * lk)
{
       
    // LAB 4: Your code here.

    return 0;
}

/* Clears a wakeup call
 *
 * Requirements:
 *
 * 1. Clear a selid from the list of wakeups.
 */
int
pipeclrsel(struct pipe *p, int * selid)
{

    // LAB 4: Your code here.

    return 0;
}
