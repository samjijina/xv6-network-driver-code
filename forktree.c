 #include "types.h"
 #include "stat.h"
 #include "user.h"
 #include "stdio.h"

 #define DEPTH 3
 
 void forktree(char *cur);
 
 void
 forkchild(char *cur, char branch)
 {
   char nxt[DEPTH+1];
 
   if (strlen(cur) >= DEPTH)
     return;
 
   snprintf(nxt, DEPTH+1, "%s%c", cur, branch);
   if (fork() == 0) {
     forktree(nxt);
     exit();
   } else
   {
       wait();
   }
 }
 
 void
 forktree(char *cur)
 {
   printf(1,"%d: I am '%s'\n", getpid(), cur);
 
   forkchild(cur, '0');
   forkchild(cur, '1');
 }
 
 int
 main(int argc, char **argv)
 {
   forktree("");
   exit();
 }
