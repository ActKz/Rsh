#include "rshd.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#define BUFF_SIZE 16000

char *welcome_str[3]={
"****************************************\n",
"** Welcome to the information server. **\n",
"****************************************\n"};

enum{READ_CMD, READ_ARG};

void rsh(int sockfd) {
  int n,i;
  char buf[BUFF_SIZE], *cptr;
  int welcome_str_len = strlen(welcome_str[0]);
  for(i=0;i<3;i++){
  if (write(sockfd, welcome_str[i], welcome_str_len) != welcome_str_len)
    err_dump("Write Socket Error!");
  }

  for (;;) {
    int st=READ_CMD;
    if(write(sockfd,"% ",2)!=2) err_dump("Write Prompt Error");
    n = read(sockfd, buf, BUFF_SIZE);
    if (n == 0)
      return;
    else if (n < 0)
      err_dump("Read Socket Error!");

    cptr = strtok(buf," ");
    while(cptr!=NULL){
    switch(st){
        case READ_CMD:
            break;
        case READ_ARG:
            break;
    }
    }

//  if (write(sockfd, buf, n) != n)
//    err_dump("Write Socket Error!");
  }
}

void err_dump(const char *str) {
  perror(str);
  exit(1);
}
