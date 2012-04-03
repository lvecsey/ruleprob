#ifndef RULEPROB_H
#define RULEPROB_H

#define remained_offline 0x1
#define remained_online 0x2
#define changed_online 0x4
#define changed_offline 0x8

typedef struct {
  int line_number;
  int availability;
} scriptrule_t;

#endif
