#include "PipeClass.h"
#include "RshClass.h"
#include "state.h"
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

Rsh::Rsh(int sockfd) { _sockfd = sockfd; }

Rsh::~Rsh() {
  while (!_lines.empty()) {
    _lines.pop();
  }
  memset(_buf, '\0', BUFF_SIZE);
}

void Rsh::printenv(const string envname) {
  char *envval = getenv(envname.c_str());
  string str;
  str += envname + "=";
  if (envval != NULL) {
    str += envval;
  } else {
    perror(strerror(errno));
  }
  str += "\n";
  write_sock(str);
}

void Rsh::setenv(const string envname, const string envval) {
  int ret = ::setenv(envname.c_str(), envval.c_str(), 1);
  if (ret < 0) {
    perror(strerror(errno));
  }
}

void Rsh::chroot(const char *root) {
  if (::chdir(_root) == -1) {
    perror(strerror(errno));
  }
}

void Rsh::general_dup2(int newfd,int oldfd){
    int rett;
    rett = dup2(newfd, oldfd);
    if (rett < 0)
      perror("DUP1 FAIL");
    else if (close(newfd) != 0)
      cerr << "Close fail: pipefd=" << newfd << strerror(errno)
           << endl;
}
void Rsh::exec_cmd(queue<group_token> cmd_args, vector<Pipe> &pipefd) {
  int childpid, pre_readfd = 0, pre_writefd = 0;
  while (!cmd_args.empty()) {
    const string CMD = cmd_args.front().argv[0];
    vector<string> vs = cmd_args.front().argv;
    if (CMD == "printenv") {
      if (vs.size() > 1) {
        for (int i = 1; i < vs.size(); i++) {
          printenv(vs[i]);
        }
      } else {
        write_sock("Usage: \n"
                   "   % printenv [EnvName]\n");
      }
    } else if (CMD == "setenv") {
      if (vs.size() > 2) {
        setenv(vs[1], vs[2]);
      } else if (vs.size() == 2) {
        setenv(vs[1], "");
      } else {
        write_sock("Usage: \n"
                   "   % setenv [EnvName] [EnvValue]\n");
      }
    } else if (CMD == "exit") {
      exit(0);
    } else {
      cout << "STATE: " << cmd_args.front().st << endl;
      Pipe pipes;
      cout << "CURR READ: " << pipes.pipes[0] << ", WRITE: " << pipes.pipes[1]
           << endl;
      if ((childpid = fork()) < 0)
        perror(strerror(errno));
      if (childpid > 0) { // parent
        int ret, push_flag = 0;
        char res[1000];

        for (int i = 0; i < pipefd.size(); i++) {
          if (pipefd[i].delay_pipe == 0) {
            pipefd[i].close_write_pipe();
            pipefd[i].close_read_pipe();
            pipefd.erase(pipefd.begin()+i);
            i--;
          }
        }
        if (cmd_args.front().st == PIPE_n ||
            cmd_args.front().st == SUPERPIPE_n) {
            int i;
            for(i=0;i<pipefd.size();i++){
                if(pipefd[i].delay_pipe == cmd_args.front().delay_pipe)
                    break;
            }
            if(i == pipefd.size()){
          pipes.delay_pipe = cmd_args.front().delay_pipe;
          pipefd.push_back(pipes);
          push_flag = 1;
            }
          cout << "pipes: " << pipefd.back().pipes[0] << ", "
               << pipefd.back().pipes[1]
               << ";delay: " << pipefd.back().delay_pipe << endl;
        } else if (cmd_args.size() == 1) {

          pipes.close_write_pipe();
          memset(res, '\0', 1000);
          while ((ret = read(pipes.pipes[0], res, 1000) > 0)) {
            write_sock(res);
          }
          pipes.close_read_pipe();
        } else {
          pipes.close_write_pipe();
          if (pre_readfd != 0 && close(pre_readfd) != 0)
            cerr << "Close failed: pipefd=" << pre_readfd << ", "
                 << strerror(errno) << endl;
          pre_readfd = pipes.pipes[0];
        }
        waitpid(childpid, &ret, 0);
        if(WIFEXITED(ret)){
            int R = WEXITSTATUS(ret);
            if(R!=0){
            if(cmd_args.front().st == PIPE_n ||
                  cmd_args.front().st == SUPERPIPE_n){
                pipes.close_write_pipe();
                pipes.close_read_pipe();
            } else if(cmd_args.size()!=1){
                pipes.close_read_pipe();
            }
            if(push_flag == 1)
                pipefd.pop_back();
            break;
            }
        }


      } else { // child
        cout << "CMD: " << CMD << endl;
        pipes.close_read_pipe();
        int rett, file;
        /* previous pipe redirection to STDIN */
        /* start */
        if (pre_readfd != 0) {
          cout << "PREV READ: " << pre_readfd << endl;
          general_dup2(pre_readfd, STDIN_FILENO);
        } else {
          for (int i = 0; i < pipefd.size(); i++) {
            if (pipefd[i].delay_pipe == 0) {
              pipefd[i].close_write_pipe();
              pipefd[i].rdr_read2in();
            }
          }
        }
        /* pipe redirection according to current state */
        /* start */
        int j;
        switch (cmd_args.front().st) {
        case NORMAL:
          pipes.rdr_err2write();
          pipes.rdr_out2write();
          break;
        case APPEND_FILE:
          file = open(cmd_args.front().file_name.c_str(),
                      O_WRONLY | O_APPEND | O_CREAT, 0666);
          if (file < 0) {
            cerr<<"FAIL!"<<endl;
            exit(1);
          }
          cout<<"SUCCESS!"<<endl;
          general_dup2(file, STDOUT_FILENO);
          break;
        case WRITE_FILE:
//        general_dup2(_sockfd, STDERR_FILENO);
          file = open(cmd_args.front().file_name.c_str(),
                      O_WRONLY | O_CREAT | O_TRUNC, 0666);
          if (file < 0) {
            exit(1);
          }
          general_dup2(file, STDOUT_FILENO);
          break;
        case PIPE_n:
//        general_dup2(_sockfd, STDERR_FILENO);
          for (j = 0; j < pipefd.size(); j++) {
            if (pipefd[j].delay_pipe == cmd_args.front().delay_pipe)
              break;
          }
          if (j == pipefd.size())
            pipes.rdr_out2write();
          else {
            pipefd[j].rdr_out2write();
            pipefd[j].close_write_pipe();
          }
          break;
        case SUPERPIPE_n:
          for (j = 0; j < pipefd.size(); j++) {
            if (pipefd[j].delay_pipe == cmd_args.front().delay_pipe)
              break;
          }
          if (j == pipefd.size()) {
            pipes.rdr_err2write();
            pipes.rdr_out2write();
          } else {
            pipefd[j].rdr_err2write();
            pipefd[j].rdr_out2write();
            pipefd[j].close_write_pipe();
          }
          break;
        }
        pipes.close_write_pipe();
        /* pipe redirection end */
        char **arg = new char *[vs.size() + 1];
        for (int i = 0; i < vs.size(); i++) {
          arg[i] = new char[vs[i].length() + 1];
          strcpy(arg[i], vs[i].c_str());
          arg[i][vs[i].length()] = '\0';
        }
        arg[vs.size()] = NULL;
        int ret = execvp(CMD.c_str(), arg);
        switch (errno) {
        case ENOENT:
          write_sock("Unknown command: [" + CMD + "].\n");
          for (int i = 0; i < vs.size(); i++)
            delete[] arg[i];
          exit(1);
          break;
        }

        for (int i = 0; i < vs.size(); i++)
          delete[] arg[i];
        exit(0);
      }
    }
    cmd_args.pop();
  }
}

queue<string> Rsh::readline() {

  print_prompt();
  read_sock();
  clear_lines();
  buf_split();
  return _lines;
}

void Rsh::buf_split() {
  char *cptr = strtok(_buf, "\r\n");
  while (cptr != NULL) {
    string tmp = cptr;
    _lines.push(tmp);
    cptr = strtok(NULL, "\r\n");
  }
}

void Rsh::clear_lines() {
  while (!_lines.empty())
    _lines.pop();
}

char *Rsh::buf() { return _buf; }

void Rsh::print_welcome() { write_sock(_welcome_str); }

void Rsh::print_prompt() { write_sock(_prompt); }

void Rsh::read_sock() {
#ifdef test
  sprintf(_buf, "aaaaaaaaaa\r\nhhhhhhhhhhhhh\r\ncccccccccccc\r\n");
#endif
  int bufSize = BUFF_SIZE;
  int next=0;
  memset(_buf, '\0', BUFF_SIZE);
          while ((_buflen = read(_sockfd, _buf+next, bufSize)) > 0) {
              if(strchr(_buf,'\n') == NULL){
                next = strlen(_buf);
              } else{
                break;
              }
          }
//  _buflen = read(_sockfd, _buf, BUFF_SIZE);
  if (_buflen < 0)
    perror(strerror(errno));
}

void Rsh::write_sock(std::string str) {
  if (write(_sockfd, str.c_str(), str.length()) != str.length())
    perror(strerror(errno));
}

void Rsh::prepare(const char *root) {
  setenv("PATH", "bin:.");
  chroot(root);
  print_welcome();
}

#ifdef test
int main() {
  Rsh tmp(STDIN_FILENO);
  tmp.setenv("PATH", "bin:.:gggggg");
  tmp.printenv("PATH");
  tmp.chroot("ras");
  system("ls");
  tmp.welcome_str();
  tmp.read_sock();
  tmp.buf_split();
  queue<string> gg = tmp.getline();
  while (!gg.empty()) {
    cout << gg.front() << endl;
    gg.pop();
  }
}
#endif
