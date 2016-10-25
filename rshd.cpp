#include "parser.h"
#include "rshd.h"
#include <fcntl.h>
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
#include <algorithm>

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
  if (::chdir(ROOT) == -1)
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
    if (buf[1] == '\n') { // Reading [ENTER]  "\r\n"
      continue;
    }
    buf[n - 1] = '\0';
    buf[n - 2] = '\0';
    cmd_parser(buf, cmds, &st, &delay_pipe);
    if (st == PIPE_n_ERROR) {
      write_sock(
          sockfd,
          "There must be one or more spaces between commands and symbols\n");
      continue;
    }
    if (st == SUPERPIPE_n_ERROR) {
      write_sock(
          sockfd,
          "There must be one or more spaces between commands and symbols\n");
      continue;
    }

    int cmds_size = cmds.size();
    int childpid, write_next[cmds_size][2], write_to_child[2];

    while (!cmds.empty()) {
      std::string CMD = cmds.front()[0];

      if (CMD.compare("printenv") == 0){
      cmds.front().erase(cmds.front().begin());
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
}
      else if (CMD.compare("setenv") == 0) {
      cmds.front().erase(cmds.front().begin());
        std::string var = cmds.front()[0];
        cmds.front().erase(cmds.front().begin());
        env[var] = cmds.front();
      } else if (CMD.compare("exit") == 0) {
        return;
      } else {
		std::vector<char*>  vc;
        int j;
        for (j = 0; j < env[PATH].size(); j++) {
          if (::access((env[PATH][j] + "/" + CMD).c_str(), X_OK) != -1)
            break;
        }
        if (j == env[PATH].size()) {
          write_sock(sockfd, "Unknown command: [" + CMD + "].\n");
          clear_queue(cmds);
          break; // read next user input
        }

		std::transform(cmds.front().begin(), cmds.front().end(), std::back_inserter(vc), convert);

        if (cmds_size == cmds.size()) { // numbered-pipe
          if (pipe(write_to_child) < 0)
            err_dump("Can't Create Pipes");
        }
        if (pipe(write_next[cmds.size() - 1]) < 0)
          err_dump("Can't Create Pipes");

		dup2(sockfd,STDOUT_FILENO);
		dup2(sockfd,STDERR_FILENO);
        if ((childpid = fork()) < 0)
          err_dump("Can't Fork");
        else if (childpid > 0) { // parent
        } else {                 // child
          if (cmds.size() == 1 && cmds_size > 1)
            child_ps(write_next[cmds.size()][0], write_next[cmds.size() - 1][1],
                     (env[PATH][j] + "/" + CMD).c_str(), vc, st);
          else if (cmds_size == cmds.size())
            child_ps(-1, write_next[cmds.size() - 1][1],
                     (env[PATH][j] + "/" + CMD).c_str(), vc, st);
          else
            child_ps(write_next[cmds.size()][0], write_next[cmds.size() - 1][1],
                     (env[PATH][j] + "/" + CMD).c_str(), vc, NORMAL);
		  exit(0);
        }
      }
      cmds.pop();
    }
  }
}
void clear_queue(std::queue<std::vector<std::string>> &q) {
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
char *convert(const std::string & s)
{
   char *pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   return pc;
}

void parent_ps(int readfd, int write_fd) {}
void child_ps(int readfd, int write_fd, const char *CMD,
              std::vector<char*> &args, int st) {
  int file;

  switch (st) {
  case NORMAL:
    break;
  case WRITE_FILE:
    file = open(args.back(), O_WRONLY | O_CREAT | O_TRUNC,0666);
	args.pop_back();
    if(file < 0)    {exit(1);}
    // redirect standard output to the file
    if(dup2(file,STDOUT_FILENO) < 0)    exit(1);


    break;
  case PIPE_n:
    break;
  case SUPERPIPE_n:
    break;
  }
	::execv(CMD,&args[0]);
}
