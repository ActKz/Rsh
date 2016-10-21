#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rshd.h"
#define DEFAULT_PORT 8888

void err_dump(const char *);

int main(int argc, char *argv[]) {
  char* pname = argv[0];
  int sockfd, newsockfd, clilen, childpid, port_num;
  struct sockaddr_in cli_addr, serv_addr;
  switch (argc) {
  case 1:
    port_num = DEFAULT_PORT;
    printf("Using default port: 8888\n");
    break;
  case 2:
    port_num = atoi(argv[1]);
    printf("Using port: %d\n",port_num);
    break;
  default:
    printf("Usage: \n");
    printf("       %s  port_num\n",pname);
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_dump("Can't open stream socket!");

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port_num);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    err_dump("Can't bind local address!");

  listen(sockfd, 5);
  for(;;) {
    clilen = sizeof(cli_addr);
    newsockfd =
        accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
      err_dump("Accept Error!");
    if ((childpid = fork()) < 0)
      err_dump("Fork Error!");
    else if (childpid == 0) {
      close(sockfd);
      rsh(newsockfd);
      exit(0);
    }
    close(newsockfd);
  }
}

void err_dump(const char *str) {
  perror(str);
  exit(1);
}
