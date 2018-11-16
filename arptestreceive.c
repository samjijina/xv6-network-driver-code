#include "types.h"
#include "user.h"

int main(void) {
  int size = 100;
  char * buff = malloc(size);
  
  if(arp_receive(buff, size) < 0) {
    printf(1, "ARP receive failed\n");
  }
  else {
    printf(1, "ARP receive succeeded.  %s\n", buff);
  }
  exit();
}
