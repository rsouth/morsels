#include "messaging.h"

struct Command parse_command(char* input) {
  struct Command cmd;
  strcpy(cmd.request, input);
  return cmd;
}

struct Msg parse_msg(char* inp) {
  struct Msg msg;
  char* tok = strtok(inp, SOH);
  strcpy(msg.type, tok);
  tok = strtok(NULL, SOH);
  strcpy(msg.data, tok);
  return msg;
}
