#include <string>
#include <unistd.h>
#include "TokenizerClass.h"
#include "RshClass.h"

Rsh::Rsh(int sockfd){
    _sockfd = sockfd;
}

char* Rsh::printenv(const char*){
}

int Rsh::setenv(const char*, const char*){
}

void Rsh::chroot(char*){
  if (::chdir(_root) == -1)
    err_dump("");

}

void Rsh::exec_cmd(queue<struct GROUP_TOKEN>){
}

const char* Rsh::welcome_str(){
    return _welcome_str.c_str();
}
