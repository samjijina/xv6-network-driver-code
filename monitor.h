#ifndef XV6_KERN_MONITOR_H
#define XV6_KERN_MONITOR_H

struct Trapframe;

// Activate the kernel monitor,
// optionally providing a trap frame indicating the current state
// (NULL if none).
void monitor(struct Trapframe *tf);

// Functions implementing monitor commands.
int mon_help(int argc, char **argv, struct Trapframe *tf);
int mon_infokern(int argc, char **argv, struct Trapframe *tf);
int mon_infopg(int argc, char **argv, struct Trapframe *tf);
int mon_backtrace(int argc, char **argv, struct Trapframe *tf);

#endif  // !XV6_KERN_MONITOR_H
