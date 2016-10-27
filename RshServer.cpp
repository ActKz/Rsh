#include "RshServer.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

RshServer::RshServer(int argc, char **argv){
  switch (argc) {
  case 1:
    port_num = DEFAULT_PORT;
    printf("Using default port: 8888\n");
    break;
  case 2:
    port_num = ::atoi(argv[1]);
    printf("Using port: %d\n",port_num);
    break;
  default:
    printf("Usage: \n");
    printf("       %s  [port_num]\n",argv[0]);
    exit(1);
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port_num);
}

RshServer::~RshServer(){}


void RshServer::create_sock(){
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      perror(strerror(errno));
}
void RshServer::bind_sock(){
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    perror(strerror(errno));

  listen(sockfd, 5);
}
void RshServer::listen_sock(){
  listen(sockfd, 5);
}

int RshServer::accept_sock(){
    clilen = sizeof(cli_addr);
    newsockfd =
        accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
      perror(strerror(errno));
    return newsockfd;
}
