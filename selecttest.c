#include "types.h"
#include "user.h"
#include "stat.h"
#include "syscall.h"
#include "select.h"

int
test0(void)
{
   fd_set s;
   int nfds = 32;
   FD_ZERO(&s);
   if (s != 0)
   {   
        printf(1, "FD_ZERO FAILED: s = %x\n", s);
        return 1;
   }
   for (int fd=0; fd<nfds; fd++)
   {
       FD_SET(fd, &s);
       if (!FD_ISSET(fd, &s))
       {
           printf(1, "FD_SET FAILED; s = %x\n", s);
           return 1;
       }
       FD_CLR(fd, &s);
       if (FD_ISSET(fd, &s))
       {
           printf(1, "FD_CLR FAILED: s = %x\n", s);
           return 1;
       }
   }
   

   return 0;
}

int
test1(void)
{
    fd_set s;
    int nfds = 5;
    int fds[2];
    
    FD_ZERO(&s);

    pipe(fds);

    if (fork() == 0)
    {
        close(fds[1]);
        fd_set readfds, writefds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fds[0],&readfds);
        if (select(nfds, &readfds, &writefds) == 0)
        {
            close(fds[0]);
            if (!FD_ISSET(fds[0],&readfds))
            {
                printf(1, "Child: select returned but read fd not set!\n");
                return 1;
            }
        }

        exit();

    } 
    else
    {
        close(fds[0]);
        fd_set readfds, writefds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fds[1],&writefds);
        if (select(nfds, &readfds, &writefds) == 0)
        {
            close(fds[1]);
            if (!FD_ISSET(fds[1],&writefds))
            {
                printf(1, "Parent: select returned but write fd not set!\n");
                wait();
                return 1;
            }
        }
        wait();
    }
    return 0;
}

int
test2(void)
{
    fd_set s;
    int nfds = 3;
    
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(0,&readfds);
    FD_SET(1,&writefds);
    FD_SET(2,&writefds);
    if (select(nfds, &readfds, &writefds) == 0)
    {
        for (int fd=0; fd<nfds; fd++)
        {
            if (FD_ISSET(fd,&readfds))
            {
                printf(1, "Console fd 0 set\n");
            }
            else
            {
                printf(1, "Console fd 0 not set\n");
            }
            if (FD_ISSET(fd,&writefds))
            {
                printf(1, "Console write fd %d set\n", fd);
            }
            else
            {
                printf(1, "Console write fd %d not set\n", fd);
            }
        }
    }
    return 0;
}

int
test3(void)
{
    fd_set s;
    int nfds = 3;
    
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(0,&readfds);
    if (select(nfds, &readfds, &writefds) == 0)
    {
        for (int fd=0; fd<nfds; fd++)
        {
            if (FD_ISSET(fd,&readfds))
            {
                printf(1, "Console fd 0 set\n");
            }
            else
            {
                printf(1, "Console fd 0 not set to read\n", fd);
            }
            if (FD_ISSET(fd,&writefds))
            {
                printf(1, "Console write fd %d set\n", fd);
            }
            else
            {
                printf(1, "Console write fd %d not set\n", fd);
            }

        }
    }
    return 0;
}

int
test3p(void)
{
    fd_set s;
    int nfds = 5;
    int fds[2];
    char wbuf[32] = "I am a write buffer\n";
    char rbuf[32];
    
    pipe(fds);
    
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    if (fork() == 0)
    {
        FD_SET(fds[0],&readfds);
        printf(1, "Selecting on Read\n");
        if (select(nfds, &readfds, &writefds) == 0)
        {
            for (int fd=0; fd<nfds; fd++)
            {
                if (FD_ISSET(fd,&readfds))
                {
                    printf(1, "fd %d set read\n", fd);
                }
                else
                {
                    printf(1, "fd %d not set to read\n", fd);
                }
                if (FD_ISSET(fd,&writefds))
                {
                    printf(1, "fd %d set write\n", fd);
                }
                else
                {
                    printf(1, "fd %d not set to write\n", fd);
                }
            }
            read(fds[0],rbuf,32);
            write(1,rbuf,32);
        }
    }
    else
    {
        printf(1, "Sleeping for 5\n");
        sleep(5);
        printf(1, "Writing to pipe\n");
        write(fds[1],wbuf,32);
        wait();
    }
    return 0;
}



int
main(void)
{

    printf(1, "test0\n");
    test0();

    printf(1, "\ntest1\n");
    test1();

    printf(1, "\ntest2\n");
    test2();

    printf(1, "\ntest3p\n");
    test3p();

    printf(1, "\ntest3\n");
    test3();
    exit();
}
