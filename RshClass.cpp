#include "RshClass.h"
#include <algorithm>
#include <errno.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

//const char *convert(const std::string & );
char *convert(const std::string &);
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

void Rsh::exec_cmd(queue<group_token> cmd_args) {
  int childpid;
  while (!cmd_args.empty()) {
      cout<<"!"<<cmd_args.front().argv.size()<<endl;
    const string CMD = cmd_args.front().argv[0];
    vector<string> vs = cmd_args.front().argv;
      cout<<"!!"<<vs.size()<<endl;
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
      if ((childpid = fork()) < 0)
        perror(strerror(errno));
      if (childpid > 0) { // parent
        int ret;
        waitpid(childpid, &ret, 0);
      } else { // child
      dup2(_sockfd, STDOUT_FILENO);
        vector<char*> arg;
        std::transform(vs.begin(), vs.end(), std::back_inserter(arg), convert);
//      for(int i=0;i<vs.size();i++){
//          cout<<vs[i]<<" ";
//          arg.push_back(strdup(vs[i].data()));
//      }

        cout<<"!!!"<<arg.size()<<endl;
        for(int i=0;i<vs.size();i++){
//          cout<<arg[i]<<"/";
            printf("%s ",arg[i]);
        }
        int ret = execvp(CMD.c_str(), arg.data());
        if (ret < 0)
          perror(strerror(errno));
        for ( size_t i = 0 ; i < arg.size() ; i++ )
                        delete [] arg[i];
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
  memset(_buf, '\0', BUFF_SIZE);
  _buflen = read(_sockfd, _buf, BUFF_SIZE);
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

char *convert(const std::string &s) {
  char *pc = new char[s.size() + 1];
  strcpy(pc, s.c_str());
  pc[s.size()] = '\0';
  return pc;
}

/*const char *convert(const std::string & s)
{
       return s.c_str();
}*/

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
