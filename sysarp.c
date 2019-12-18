/**
 *author: Anmol Vatsa<anvatsa@cs.utah.edu>
 *
 *system call to send an ARP request
 *expects a char* IP address as argument
 */

#include "types.h"
#include "defs.h"
#include "e1000.h"

int sys_arp(void) {
  char *ipAddr, *interface, *arpResp;
  int size;

  if(argstr(0, &interface) < 0 || argstr(1, &ipAddr) < 0 || argint(3, &size) < 0 || argptr(2, &arpResp, size) < 0) {
    cprintf("ERROR:sys_createARP:Failed to fetch arguments");
    return -1;
  }

  if(send_arpRequest(interface, ipAddr, arpResp) < 0) {
    cprintf("ERROR:sys_createARP:Failed to send ARP Request for IP:%s", ipAddr);
    return -1;
  }

  return 0;
}

int sys_arpserv(void) {
  char *interface;

  if(argstr(0, &interface) < 0) {
    cprintf("ERROR:sys_arpserv:Failed to fetch arguments");
    return -1;
  }

  return recv_arpRequest(interface);
}

int sys_arp_receive(void) {
  char *buff;
  int size;

  if(argint(1, &size) < 0 || argptr(0, &buff, size) < 0) {
    cprintf("ERROR: Failed to fetch arguments from arp_receive\n");
    return -1;
  }

  int return_size = e1000_receive(buff, size);
  if(return_size == 0) {
    cprintf("Error: e1000 receive failed\n");
    return -1;
  }

  return return_size;
}
