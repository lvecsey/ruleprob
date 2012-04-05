#include <stdio.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <time.h>

#include "show_address.h"

#include "cmd_magic.h"

int main(int argc, char *argv[]) {

  int len;
  int r;

  unsigned char packet[256];
  struct sockaddr_in6 sa6;
  int s;

  unsigned short destination_port = 4985;

  unsigned short ns_destination_port;

  unsigned char *ipv6_address_string = argc>3 ? argv[3] : NULL;

  long int server_no = argc>2 ? strtol(argv[2], NULL, 10) : -1;

  char *cmd_string = argc>1 ? argv[1] : NULL;

  unsigned char ipv6_address[16];

  int sendto_retval;

  struct timespec ts;

  u_int64_t cmd = 0;

  int retval;

  if (cmd_string == NULL || server_no < 0) {
    fprintf(stderr, "%s: Usage: rp-send [turnon|turnoff] server_no ipv6_address\n", __FUNCTION__);
    return -1;
  }

  if (ipv6_address_string!=NULL) {
    retval = sscanf(ipv6_address, "%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x", ipv6_address, ipv6_address+1, ipv6_address+2, ipv6_address+3, ipv6_address+4, ipv6_address+5, ipv6_address+6, ipv6_address+7, ipv6_address+8, ipv6_address+9, ipv6_address+10, ipv6_address+11, ipv6_address+12, ipv6_address+13, ipv6_address+14, ipv6_address+15);
    printf("%s: ipv6 address conversion (sscanf) returned %d.\n", __FUNCTION__, retval);
  }
  else {
    fprintf(stderr, "%s: Please specify an ipv6 address.\n", __FUNCTION__);
    return -1;
  }

  if (!strncmp(cmd_string, "turnon", 6)) cmd = cmd_turnon;
  if (!strncmp(cmd_string, "turnoff", 7)) cmd = cmd_turnoff;

  if (cmd != cmd_turnon && cmd != cmd_turnoff) {
    fprintf(stderr, "%s: Must specify either turnon or turnoff.\n", __FUNCTION__);
    return -1;
  }

  s = socket(AF_INET6,SOCK_DGRAM,0);
  if (s == -1) {
    perror("socket");
    return -1;
  }

  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&retval, sizeof(retval)) < 0) {
    perror("setsockopt");
    return -1;
  }

  memset(&sa6,0,sizeof(sa6));
  memcpy(sa6.sin6_addr.s6_addr, ipv6_address, 16);

  ns_destination_port = htons(destination_port);
  memcpy(&sa6.sin6_port, &ns_destination_port, sizeof(unsigned short));
  sa6.sin6_family = AF_INET6;

  len = sizeof(sa6);
  
  clock_gettime(CLOCK_REALTIME, &ts);

  memcpy(packet, &cmd, sizeof(u_int64_t));
  memcpy(packet + sizeof(u_int64_t), &server_no, sizeof(u_int64_t));

  memcpy(packet + sizeof(u_int64_t) * 2, &ts, sizeof(struct timespec));

  r = sizeof(u_int64_t) * 2 + sizeof(struct timespec);

  show_address(ipv6_address);

  sendto_retval = sendto(s,packet,r,0,(const struct sockaddr *) &sa6,len);
  if (sendto_retval==-1) {
    perror("sendto");
    return -1;
  }

  return 0;

}
