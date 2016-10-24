#include "rshd.h"
#include <ctype.h>
#include <netinet/in.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <map>

#define BUFF_SIZE 16000

const std::string welcome_str[3] = {
    "****************************************\n",
    "** Welcome to the information server. **\n",
    "****************************************\n"};

enum {
  READ_CMD,
  READ_ARG,
  PIPE_n,
  SUPERPIPE_n,
  PIPE_n_ERROR,
  SUPERPIPE_n_ERROR
};

void rsh(int sockfd) {
  int n, i;
  char buf[BUFF_SIZE];
    std::vector<std::string> cmd_arg;
    std::queue<std::vector<std::string>> cmds;
    std::map<std::string,std::vector<std::string>> env;
    env["PATH"] = {"bin","."};

  for (i = 0; i < 3; i++) {
    if (::write(sockfd, welcome_str[i].c_str(), welcome_str[i].length()) !=
        welcome_str[i].length())
      err_dump("Write Socket Error!");
  }

  for (;;) {
    int st = READ_CMD, delay_pipe;
    char *cptr;


    if (write(sockfd, "% ", 2) != 2)
      err_dump("Write Prompt Error");
    n = read(sockfd, buf, BUFF_SIZE);
    if (n == 0)
      return;
    else if (n < 0)
      err_dump("Read Socket Error!");
    //  if (write(sockfd, buf, n) != n)
    //    err_dump("Write Socket Error!");
    //    cmd_parser(buf, &cmd_arg, &st, &delay_pipe,sockfd);
//  ::write(sockfd,buf+n-2,1);
    buf[n-2] = '\0';
    cptr = strtok(buf, " ");
    while (cptr != NULL) {

      switch (cptr[0]) {
      case '|':
        cmds.push(cmd_arg);
        cmd_arg.clear();
        if (isdigit(cptr[1])) {
          st = PIPE_n;
          delay_pipe = atoi(cptr);
        } else if (cptr[1] == '\0')
          st = READ_CMD;
        else
          st = PIPE_n_ERROR;

        break;
      case '!':
        cmds.push(cmd_arg);
        if (isdigit(cptr[1])) {
          st = SUPERPIPE_n;
          delay_pipe = atoi(cptr);
        } else
          st = SUPERPIPE_n_ERROR;
        break;
      default:
        cmd_arg.push_back(cptr);
        st = READ_ARG;
      }
      cptr = strtok(NULL, " ");
    }
    cmds.push(cmd_arg);
    cmd_arg.clear();

    while (!cmds.empty()) {
        std::string CMD = cmds.front()[0];
        cmds.front().erase(cmds.front().begin());
      if (CMD.compare("printenv") == 0)
        if(env.find(cmds.front()[0])==env.end()){ // not found
            std::string res = "ERROR: "+cmds.front()[0]+" not set!";
            ::write(sockfd,res.c_str(),res.length());
        }
        else{ //found
            std::string res = cmds.front()[0]+"=";
            for(int j=0;j<env[cmds.front()[0]].size();j++){
                res += env[cmds.front()[0]][j];
                if(j!=env[cmds.front()[0]].size()-1)
                    res += ":";
            }
            ::write(sockfd,res.c_str(),res.length());
        }
      else if (CMD.compare("setenv") == 0){
        std::string var = cmds.front()[0];
        cmds.front().erase(cmds.front().begin());
        env[var] = cmds.front();
        }
      else if (CMD.compare("exit") == 0){
//    ::write(sockfd, cmds.front()[0].c_str(), cmds.front()[0].length());
        return;}
      else {
        //          int childpid, pipe1[2], pipe2[2];
        //          if(pipe(pipe1)<0||pipe(pipe2)<0) err_dump("Can't Create
        //          Pipe!");
        //          if((childpid=fork())<0){
        //              err_dump("Can't Fork!");
        //          } else if (childpid>0){ //parent

        //          } else { //child
        //          }
      }
      ::write(sockfd,"\n",1);
      cmds.pop();
    }
  }
}

void cmd_parser(
    char *buf,
    std::queue<std::tuple<std::string, std::vector<std::string>>> *cmds,
    int *st, int *delay_pipe, int sockfd) {}

void err_dump(const char *str) {
  perror(str);
  exit(1);
}
