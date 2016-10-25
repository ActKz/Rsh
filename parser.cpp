#include<vector>
#include<queue>
#include<stdlib.h>
#include<string.h>
#include"rshd.h"
void cmd_parser(
    char *buf,
    std::queue<std::vector<std::string>> &cmds,
    int *st, int *delay_pipe) {
    char *cptr;
  std::vector<std::string> cmd_arg;
    cptr = strtok(buf, " ");
    while (cptr != NULL) {

      switch (cptr[0]) {
      case '|':
        cmds.push(cmd_arg);
        cmd_arg.clear();
        if (isdigit(cptr[1])) {
          *st = PIPE_n;
          *delay_pipe = atoi(cptr);
        } else if (cptr[1] == '\0')
          ;
        else
          *st = PIPE_n_ERROR;
        break;
      case '!':
        cmds.push(cmd_arg);
        if (isdigit(cptr[1])) {
          *st = SUPERPIPE_n;
          *delay_pipe = atoi(cptr);
        } else
          *st = SUPERPIPE_n_ERROR;
        break;
      case '>':
        *st = WRITE_FILE;
        break;
      default:
        cmd_arg.push_back(cptr);
      }
      cptr = strtok(NULL, " ");
    }
    cmds.push(cmd_arg);
    cmd_arg.clear();
}
