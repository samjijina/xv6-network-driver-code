#ifndef DEFS_H_
#define DEFS_H_
struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct stat;
struct superblock;
struct page_info;

int             timed_sleep(int n);

// arp.c
int 		send_arpRequest(char* , char* , char* );
int             recv_arpRequest(char *interface);

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// console.c
void            consoleinit(void);
int     cprintf(const char *fmt, ...);
//void            cprintf(char*, ...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));

// exec.c
int             exec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);

/**
 * Indicates if a file can be written without blocking.
 * @param {struct file *} f - the file to be checked
 * @return 0 for true, >0 for false, -1 for error.
 */
int             filewriteable(struct file *);

/**
 * Indicates if a file can be read without blocking.
 * @param {struct file *} f - the file to be checked
 * @return 0 for true, >0 for false, -1 for error.
 */
int             filereadable(struct file *);

int             fileselect(struct file *, int *, struct spinlock *);

int             fileclrsel(struct file *, int *);

// fs.c
void            readsb(int dev, struct superblock *sb);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit(int dev);
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, uint, uint);


/**
 * Indicates if a inode can be read without blocking.
 * @param {struct inode *} i - the inode to be checked
 * @param {uint} off - offset into the file (unused for devices)
 * @return 0 for true, >0 for false, -1 for error.
 */
int             readablei(struct inode*, uint);

/**
 * Indicates if a inode can be written without blocking.
 * @param {struct inode *} i - the inode to be checked
 * @param {uint} off - offset into the file (unused for devices)
 * @return 0 for true, >0 for false, -1 for error.
 */
int             writeablei(struct inode*, uint);

int             selecti(struct inode*, int *, struct spinlock * lk);

int             clrseli(struct inode*, int *);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// ioapic.c
void            ioapicenable(int irq, int cpu);
extern uchar    ioapicid;
void            ioapicinit(void);

// kalloc.c
int 									kinsert(pde_t *pgdir, struct page_info *pp, char *va, int perm);
void 									kremove(pde_t *pgdir, void *va);
struct page_info * 		klookup(pde_t *pgdir, void *va, pte_t **pte_store);
char*           			kalloc(void);
void            			kfree(char*);
void            			kinit1(void*, void*);
void            			kinit2(void*, void*);

// kbd.c
void            kbdintr(void);

// lapic.c
void            cmostime(struct rtcdate *r);
int             cpunum(void);
extern volatile uint*    lapic;
void            lapiceoi(void);
void            lapicinit(void);
void            lapicstartap(uchar, uint);
void            microdelay(int);

// log.c
void            initlog(int dev);
void            log_write(struct buf*);
void            begin_op();
void            end_op();

// mp.c
extern int      ismp;
void            mpinit(void);

// picirq.c
void            picenable(int);
void            picinit(void);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);


/**
 * Indicates if a pipe can be written without blocking.
 * @param {struct pipe *} p - the pipe to be checked
 * @return 0 for true, >0 for false, -1 for error.
 */
int             pipewriteable(struct pipe*);


/**
 * Indicates if a pipe can be read without blocking.
 * @param {struct pipe *} p - the pipe to be checked
 * @return 0 for true, >0 for false, -1 for error.
 */
int             pipereadable(struct pipe*);

int             pipeselect(struct pipe*, int *, struct spinlock *);

int             pipeclrsel(struct pipe*, int *);

//PAGEBREAK: 16
// proc.c
void            exit(void);
int             fork(void);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(void);
void            wakeup(void*);
void            yield(void);

// swtch.S
void            swtch(struct context**, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uint*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(uint, int*);
int             fetchstr(uint, char**);
void            syscall(void);

// timer.c
void            timerinit(void);

// trap.c
void            idtinit(void);
extern uint     ticks;
void            tvinit(void);
extern struct spinlock tickslock;

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartputc(int);
void		uartprintcstr(char*);

// vm.c
void 						tlb_invalidate(pde_t *pgdir, void *va);
void            seginit(void);
void            kvmalloc(void);
pde_t*          setupkvm(void);
char*           uva2ka(pde_t*, char*);
int             allocuvm(pde_t*, uint, uint);
int             deallocuvm(pde_t*, uint, uint);
void            freevm(pde_t*);
void            inituvm(pde_t*, char*, uint);
int             loaduvm(pde_t*, char*, struct inode*, uint, uint);
pde_t*          copyuvm(pde_t*, uint);
void            switchuvm(struct proc*);
void            switchkvm(void);
int             copyout(pde_t*, uint, void*, uint);
void            clearpteu(pde_t *pgdir, char *uva);
void * 		mmio_map_region(uint pa, uint size);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#define assert(x) \
  do { if (!(x)) panic("assertion failed!"); } while (0)

#endif
