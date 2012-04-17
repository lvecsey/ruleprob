
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

#include <endian.h>

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ruleprob.h"

#include "cmd_magic.h"

int action_call(char *cmd_string, u_int64_t server_no, float probability) {

  char string[80];

  int retval;

  sprintf(string, "./iptables-script.sh %s %lu %0.1f", cmd_string, server_no, probability);

  retval = system(string);

  printf("%s: System command for cmd=%s server_no=%lu probability=%f returned %d.\n", __FUNCTION__, cmd_string, server_no, probability, retval);

  return 0;

}

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

  u_int64_t cmd_be64, server_no_be64;

  float probability = 0.5;

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

  servers[0].availability = remained_offline;
  servers[1].availability = remained_offline;

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

      memcpy(&cmd_be64, packet, sizeof(u_int64_t));
      memcpy(&server_no_be64, packet + sizeof(u_int64_t), sizeof(u_int64_t));

      cmd = be64toh(cmd_be64);
      server_no = be64toh(server_no_be64);

      clock_gettime(CLOCK_REALTIME, &ts);

      printf("%s: Received packet, length=%d\n", __FUNCTION__, r);

      if (cmd == cmd_turnon || cmd == cmd_turnoff) {

	char *cmd_string = cmd == cmd_turnon ? "turnon" : "turnoff";

	if (server_no == 1) {

	  probability = (cmd == cmd_turnon) ? 1.0f : 0.0f;

	}

	if (server_no == 0) {

	  probability = (servers[1].availability == changed_online) ? 0.5f : 1.0f;

	}

	action_call(cmd_string, server_no, probability);

	if (server_no < num_servers) {
	  servers[server_no].availability = (cmd == cmd_turnon) ? changed_online : changed_offline;
	}

	if (cmd == cmd_turnoff && server_no == 1 && servers[0].availability == changed_online) {

	  action_call("turnon", 0, 1.0f);

	}

	if (cmd == cmd_turnon && server_no == 1 && servers[0].availability == changed_online) {

	  action_call("turnon", 0, 0.5f);

	}

      }

    }

  }

  close(s);

  return 0;

}
