#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEFAULT_PORT 8888

class RshServer{
    public:
        RshServer(int, char**);
        ~RshServer();
        void create_sock();
        void bind_sock();
        void listen_sock();
        int accept_sock();
        int clilen,newsockfd,childpid,sockfd;
        struct sockaddr_in cli_addr;
        void prepare();
    private:
        int port_num;
        struct sockaddr_in serv_addr;
};
