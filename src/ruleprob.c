
#include <stdio.h>
#include <assert.h>

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>

#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "ruleprob.h"

u_int64_t cmd_turnon = 0x20485041;
u_int64_t cmd_turnoff = 0x20485240;

int main(int argc, char *argv[]) {

  unsigned char packet[256];

  struct sockaddr_in6 sa6;

  int s;

  int num_servers = 2;

  scriptrule_t *servers = malloc(sizeof(scriptrule_t) * num_servers); 

  int len;

  int r;

  unsigned short listen_port = 4985;

  unsigned short ns_listen_port;

  struct timespec ts;

  u_int64_t cmd, server_no;

  char string[80];

  int retval;

  s = socket(AF_INET6,SOCK_DGRAM,0);
  if (s == -1) {
    perror("socket");
    return -1;
  }

  if (servers==NULL) {
    fprintf(stderr, "%s: Trouble with call to malloc.\n", __FUNCTION__);
    return -1;
  }
  
  if (0 == system(NULL)) {
    fprintf(stderr, "%s: Shell is not available.\n", __FUNCTION__);
    return -1;
  }

  memset(&sa6,0,sizeof(sa6));
  ns_listen_port = htons(listen_port);
  memcpy(&sa6.sin6_port, &ns_listen_port, sizeof(unsigned short));
  sa6.sin6_family = AF_INET6;

  if (bind(s,(struct sockaddr *) &sa6,sizeof sa6) == -1) {
    perror("bind");
    return -1;
  }

  for (;;) {
    len = sizeof(sa6);
    r = recvfrom(s,packet,sizeof packet,0,(struct sockaddr *) &sa6,&len);
    if (r >= sizeof(u_int64_t) * 2) {

      memcpy(&cmd, packet, sizeof(u_int64_t));
      memcpy(&server_no, packet + sizeof(u_int64_t), sizeof(u_int64_t));

      packet[0] = 's';
      clock_gettime(CLOCK_REALTIME, &ts);

      packet[0] = 3;
      packet[1] = 78;
      packet[2] = 9;
      packet[3] = 5;

      printf("%s: Received packet, length=%d\n", __FUNCTION__, r);

      if (cmd == cmd_turnon || cmd == cmd_turnoff) {

	char *cmd_string = cmd == cmd_turnon ? "turnon" : "turnoff";

	sprintf(string, "./iptables-script.sh %s %lu", cmd_string, server_no);

	retval = system(string);

	printf("%s: System command for cmd=%s server_no=%lu returned %d.\n", __FUNCTION__, cmd_string, server_no);

      }

    }
  }


  return 0;

}
