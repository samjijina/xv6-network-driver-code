#ifndef SELECT_H
#define SELECT_H

#include "types.h"
#include "param.h"

struct spinlock;

struct selproc
{
  int * sel[NSELPROC];
  struct spinlock * lk[NSELPROC];
  int selcount;
};

void initselproc(struct selproc *);
void clearselid(struct selproc *, int *);
void addselid(struct selproc *, int*, struct spinlock * lk);
void wakeupselect(struct selproc *);

static inline void _fd_set(int fd, fd_set* set)
{
    *set |= (1 << fd);
}

static inline int _fd_isset(int fd, fd_set* set)
{
    return *set & (1 << fd);
}

static inline void _fd_clr(int fd, fd_set* set)
{
    *set &= ~(1 << fd);
}

static inline void _fd_zero(fd_set* set)
{
    *set = 0;
}

#define FD_SET(fd, set) _fd_set(fd, set)

#define FD_ISSET(fd, set) _fd_isset(fd,set)

#define FD_CLR(fd, set) _fd_clr(fd, set)

#define FD_ZERO(set) _fd_zero(set)

#endif
