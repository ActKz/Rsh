#include "RshServer.h"
#include "RshClass.h"
#include "TokenizerClass.h"
#include <string>
#include <queue>
#define ROOT "ras"

void rsh(int);

int main(int argc, char *argv[]){
   RshServer server(argc, argv);
   server.create_sock();
   server.bind_sock();
   server.listen_sock();
  for(;;) {
    server.accept_sock();
    server.clilen = sizeof(server.cli_addr);
    server.newsockfd = server.accept_sock();
    if ((server.childpid = fork()) < 0)
        perror(strerror(errno));
    else if (server.childpid == 0) {
      close(server.sockfd);
      rsh(server.newsockfd);
      _exit(0);
    }
    close(server.newsockfd);
  }
}

void rsh(int sockfd){
      Rsh rsh(sockfd);
      rsh.chroot(ROOT);
      rsh.print_welcome();
      for(;;){
          queue<string> tmp;
          rsh.print_prompt();
          rsh.read_sock();
          rsh.buf_split();
          tmp = rsh.getline();
          while(!tmp.empty()){
              Tokenizer tokens(tmp.front().c_str());
              tokens.parse();
              queue<group_token*> parsed_token = tokens.cmd_args();
              rsh.exec_cmd(parsed_token);
              tmp.pop();
          }
      }
}
