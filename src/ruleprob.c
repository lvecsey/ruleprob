
#include <stdio.h>
#include <assert.h>

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>

#include <errno.h>

#include "ruleprob.h"

int main(int argc, char *argv[]) {

  int num_servers = 2;

  scriptrule_t *servers = malloc(sizeof(scriptrule_t) * num_servers);

  if (servers==NULL) {
    fprintf(stderr, "%s: Trouble with call to malloc.\n", __FUNCTION__);
    return -1;
  }

  

  return 0;

}
