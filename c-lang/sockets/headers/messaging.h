#ifndef MESSAGING_H
#define MESSAGING_H

#include <string.h>

#define CMD_PUB_TIME "publish-time\n"
#define SOH "|"

struct Msg {
  char type[5];
  char data[100];
};

struct Msg parse_msg(char* inp);

struct Command {
  // todo can we malloc this to allow arbitrary size?
  char request[100];
};

struct Command parse_command(char* input);

#endif
