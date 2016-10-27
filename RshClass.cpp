#include "RshClass.h"
#include <errno.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <string>
#include <unistd.h>
#include <algorithm>
using namespace std;

char* convert(const std::string & );
Rsh::Rsh(int sockfd) { _sockfd = sockfd; }

Rsh::~Rsh() {}

void Rsh::printenv(const string envname) {
  char *envval = getenv(envname.c_str());
  cout << envname << "=";
  if (envval != NULL) {
    cout << envval;
  } else {
    perror(strerror(errno));
  }
  cout << endl;
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

void Rsh::exec_cmd(queue<group_token*> cmd_args) {
  int childpid;
  while (!cmd_args.empty()) {
    const string &CMD = (*cmd_args.front()).argv[0];
    vector<string> &vs = (*cmd_args.front()).argv;
    vector<char*> argv;
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
	  std::transform(vs.begin(), vs.end(), std::back_inserter(argv), convert);
      if ((childpid = fork()) < 0)
        perror(strerror(errno));
      if (childpid > 0) { // parent
      } else {            // child
        ::execvp(CMD.c_str(),&argv[0]);
        exit(0);
      }
    }
  }
}

queue<string> Rsh::getline() { return _lines; }

void Rsh::buf_split() {
  char *cptr = strtok(_buf, "\r\n");
  while (cptr != NULL) {
    _lines.push(cptr);
    cptr = strtok(NULL, "\r\n");
  }
}

char *Rsh::buf() { return _buf; }

void Rsh::print_welcome() { cout << _welcome_str; }

void Rsh::print_prompt() { write_sock(_prompt); }

void Rsh::read_sock() {
#ifdef test
  sprintf(_buf, "aaaaaaaaaa\r\nhhhhhhhhhhhhh\r\ncccccccccccc\r\n");
#endif
  _buflen = read(_sockfd, _buf, BUFF_SIZE);
  if (_buflen == 0)
    return;
  else if (_buflen < 0)
    perror(strerror(errno));
}

void Rsh::write_sock(std::string str) {
  if (write(_sockfd, str.c_str(), str.length()) != str.length())
    perror(strerror(errno));
}

char* convert(const std::string & s)
{
   char *pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   printf("%s",pc);
   return pc;
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
