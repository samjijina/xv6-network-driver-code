#include "types.h"
#include "user.h"

int main(void) {
  while (1) {
    int status = arpserv("mynet0");
    printf(1, "Served ARP request!\n");
    if(status < 0)
      break;
    else
	printf(1, "Status: %d\n", status);
  }
  exit();
}
