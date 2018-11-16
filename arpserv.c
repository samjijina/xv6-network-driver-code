#include "types.h"
#include "user.h"

int main(void) {
  while (1) {
    arpserv("mynet0");
    printf(1, "Served ARP request!\n");
  }
  exit();
}
