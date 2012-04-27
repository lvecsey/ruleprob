#include <stdio.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <endian.h>

#include <netdb.h>

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

  u_int64_t sn;

  u_int64_t cmd_be32, sn_be32;

  int retval;

  struct addrinfo hints;

  struct addrinfo *res;

  int close_retval;

  if (cmd_string == NULL || server_no < 0) {
    fprintf(stderr, "%s: Usage: rp-send [turnon|turnoff] server_no ipv6_address\n", __FUNCTION__);
    return -1;
  }

  if (ipv6_address_string!=NULL) {

    struct addrinfo *rp;

    hints.ai_flags = 0;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 17;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    retval = getaddrinfo(ipv6_address_string, NULL, &hints, &res);
    
    if (retval != 0) {
      fprintf(stderr, "%s: Trouble with call to getaddrinfo.\n", __FUNCTION__);
      fprintf(stderr, "%s: gai_strerror = %s\n", __FUNCTION__, gai_strerror(retval));
      return -1;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {

      memcpy(ipv6_address, ((char*) rp->ai_addr) + 8, sizeof(ipv6_address));

      break;

    }

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

  sn = server_no;

  cmd_be32 = htonl(cmd);
  sn_be32 = htonl(sn);

  memcpy(packet, &cmd_be32, sizeof(u_int32_t));
  memcpy(packet + sizeof(u_int32_t), &sn_be32, sizeof(u_int32_t));

  memcpy(packet + sizeof(u_int64_t) * 2, &ts, sizeof(struct timespec));

  r = sizeof(u_int32_t) * 2 + sizeof(struct timespec);

  show_address(ipv6_address);

  sendto_retval = sendto(s,packet,r,0,(const struct sockaddr *) &sa6,len);
  if (sendto_retval==-1) {
    perror("sendto");
    return -1;
  }

  close_retval = close(s);
  if (close_retval==-1) {
    fprintf(stderr, "%s: Expected clean socket close and got retval=%d.\n", __FUNCTION__, close_retval);
    return -1;
  }

  return 0;

}
