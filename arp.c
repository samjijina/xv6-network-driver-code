/**
 *author: Anmol Vatsa<anvatsa@cs.utah.edu>
 *
 *kernel code to send recv arp request responses
 */

#include "types.h"
#include "defs.h"
#include "arp_frame.h"
#include "e1000.h"
#include "nic.h"

static int block_until_arp_reply(struct ethr_hdr *arpReply) {
  /**
   *TODO: repeated sleep. wake up on each network interrupt.
   *      check for ARP reply for this request.
   *      If received, unblock. else, sleep again.
   */
  return 0;
}

int send_arp(char* interface, const char *myIpAddr, uint8_t *dst_mac, char* ipAddr, bool is_reply, uint8_t *macMsg) {
  cprintf("Create arp request for ip:%s over Interface:%s\n", ipAddr, interface);

  struct nic_device *nd;
  if(get_device(interface, &nd) < 0) {
    cprintf("ERROR:send_arpRequest:Device not loaded\n");
    return -1;
  }

  struct ethr_hdr eth;
  create_eth_arp_frame(nd->mac_addr, myIpAddr, is_reply ? dst_mac : NULL, ipAddr, is_reply, macMsg, &eth);
  e1000_transmit((char *)&eth, sizeof(eth)-2);

  return 0;
}

int recv_arp(const char *myIpAddr, bool expected_reply, uint8_t *received_from_mac, char *received_from_ip, char *arpMsgMac) {
  //cprintf("Wait for arp over Interface:%s\n", interface);

  struct ethr_hdr eth;

  uint8_t arpMsgMacBytes[6];


  while (true) {
    int status = e1000_receive((char*)&eth, sizeof(eth) - 2);
    if (status < 0) {
      cprintf("Error: in e1000 receive\n");   
      return -1;
    } else if(status) {
      cprintf("received arp message!\n");
      if (parse_arp_packet(&eth, myIpAddr, expected_reply, received_from_mac, received_from_ip, arpMsgMacBytes) < 0) {
        continue;
      }

      if (arpMsgMac) {
        unpack_mac(arpMsgMacBytes, arpMsgMac);
        arpMsgMac[17] = '\0';
      }

      return 0; // successfully received
    }

    timed_sleep(10);
  }
}

int send_arpRequest(char *interface, char *ipAddr, char *arpResp) {
  const char *sender_ip = "192.168.1.1";
  uint8_t emptymac[6];

  cprintf("Sending ARP request\n");
  send_arp(interface, sender_ip, NULL, ipAddr, false, emptymac);

  cprintf("Sent ARP request\n");

  //struct ethr_hdr arpResponse;
  if(recv_arp(sender_ip, true, NULL, NULL, arpResp) < 0) {
    cprintf("ERROR:send_arpRequest:Failed to recv ARP response over the NIC\n");
    return -3;
  }

  return 0;
}

int recv_arpRequest(char *interface) {
  const char *receiver_ip = "192.168.2.1";

  struct nic_device *nd;
  if(get_device(interface, &nd) < 0) {
    cprintf("ERROR:recv_arp:Device not loaded\n");
    return -1;
  }

  cprintf("Receiving ARP request\n");

  uint8_t received_from_mac[6];
  char received_from_ip[16];

  if(recv_arp(receiver_ip, false, received_from_mac, received_from_ip, NULL) < 0) {
    cprintf("ERROR:recv_arpRequest:Failed to recv ARP request over the NIC\n");
    return -3;
  }

  send_arp(interface, receiver_ip, received_from_mac, received_from_ip, true, nd->mac_addr);
  return 0;
}
