#include "types.h"
#include "param.h"
#include "select.h"
#include "spinlock.h"
#include "defs.h"

extern void wakeup(void *);


void initselproc(struct selproc * s)
{
  s->selcount = 0;
  for (int i=0; i<NSELPROC; i++)
  {
      s->sel[i] = 0;
  }
}

void clearselid(struct selproc *s, int * selid)
{
    for (int i=0; i<NSELPROC; i++)
    {
        if (s->sel[i] == selid)
        {
            s->sel[i] = 0;
            s->selcount--;
        }
    }
}

void addselid(struct selproc *s, int * selid, struct spinlock * lk)
{
    for (int i=0; i<NSELPROC; i++)
    {
        if (s->sel[i] == 0)
        {
            s->sel[i] = selid;
            s->lk[i] = lk;
            s->selcount++;
            break;
        }
    }
}

void wakeupselect(struct selproc * s)
{
    for (int i=0; i<NSELPROC; i++)
    {
        if (s->sel[i])
        {
            acquire(s->lk[i]);
            wakeup((void*)s->sel[i]);
            release(s->lk[i]);
        }
    }
}
