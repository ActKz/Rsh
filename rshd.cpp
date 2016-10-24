#include "rshd.h"
#include "parser.h"
#include <ctype.h>
#include <map>
#include <netinet/in.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFF_SIZE 16000
#define PATH "PATH"
#define ROOT "ras"
const std::string welcome_str[3] = {
    "****************************************\n",
    "** Welcome to the information server. **\n",
    "****************************************\n"};

void rsh(int sockfd) {
  int n, i;
  char buf[BUFF_SIZE];
  std::queue<std::vector<std::string>> cmds;
  std::map<std::string, std::vector<std::string>> env;
  env[PATH] = {"bin", "."};

  for (i = 0; i < 3; i++) {
    write_sock(sockfd, welcome_str[i]);
  }
  if(::chdir(ROOT)==-1)
    err_dump("");

  for (;;) {
    int st = NORMAL, delay_pipe;
    char *cptr;

    write_sock(sockfd, "% ");
    n = read(sockfd, buf, BUFF_SIZE);
    if (n == 0)
      return;
    else if (n < 0)
      err_dump("Read Socket Error!");
    //  if (write(sockfd, buf, n) != n)
    //    err_dump("Write Socket Error!");
    if(buf[1]=='\n'){ // Reading [ENTER]  "\r\n"
        continue;
    }
    buf[n - 1] = '\0';
    buf[n - 2] = '\0';
    cmd_parser(buf,cmds,&st,&delay_pipe);

    while (!cmds.empty()) {
      std::string CMD = cmds.front()[0];
      cmds.front().erase(cmds.front().begin());
      if (CMD.compare("printenv") == 0)
        if (env.find(cmds.front()[0]) == env.end()) { // key not found
          std::string res = "ERROR: " + cmds.front()[0] + " not set!\n";
          write_sock(sockfd, res);
        } else { // found
          std::string res = cmds.front()[0] + "=";
          for (int j = 0; j < env[cmds.front()[0]].size(); j++) {
            res += env[cmds.front()[0]][j];
            if (j != env[cmds.front()[0]].size() - 1)
              res += ":";
          }
          write_sock(sockfd, res + "\n");
        }
      else if (CMD.compare("setenv") == 0) {
        std::string var = cmds.front()[0];
        cmds.front().erase(cmds.front().begin());
        env[var] = cmds.front();
      } else if (CMD.compare("exit") == 0) {
        return;
      } else {
        int j;
        for (j = 0; j < env[PATH].size(); j++){
          if (::access((env[PATH][j] + "/" + CMD).c_str(), X_OK) != -1)
            break;
        }
        if (j == env[PATH].size()){
          write_sock(sockfd, "Unknown command: [" + CMD + "].\n");
          clear_queue(cmds);
          break; // read next user input
        }
        //          int childpid, pipe1[2], pipe2[2];
        //          if(pipe(pipe1)<0||pipe(pipe2)<0) err_dump("Can't Create
        //          Pipe!");
        //          if((childpid=fork())<0){
        //              err_dump("Can't Fork!");
        //          } else if (childpid>0){ //parent

        //          } else { //child
        //          }
      }
      cmds.pop();
    }
  }
}
void clear_queue(std::queue<std::vector<std::string>> &q){
    std::queue<std::vector<std::string>> empty;
    std::swap(q, empty);
}

void write_sock(int sockfd, std::string str) {
  if (::write(sockfd, str.c_str(), str.length()) != str.length())
    err_dump("Write to socket fail!");
}

void err_dump(const char *str) {
  perror(str);
  exit(1);
}

