#include "RshServer.h"
#include "RshClass.h"
#include "TokenizerClass.h"
#include <string>
#include <queue>
#define ROOT "ras"
#include <iostream>
#include <errno.h>
using namespace std;
void rsh(int);

int main(int argc, char *argv[]){
    int newsockfd;
   RshServer server(argc, argv);
   server.prepare();
  for(;;) {
    newsockfd = server.accept_sock();
    if ((server.childpid = fork()) < 0)
        perror(strerror(errno));
    else if (server.childpid == 0) {
      close(server.sockfd);
      rsh(newsockfd);
      _exit(0);
    }
    close(newsockfd);
  }
}

void rsh(int sockfd){
      Rsh rsh(sockfd);
      queue<string> tmp;
      rsh.prepare(ROOT);
      for(;;){
          tmp=rsh.readline();
//        cout<<"!"<<tmp.size()<<endl;
          while(!tmp.empty()){
//            cout<<"~"<<tmp.front()<<endl;
              Tokenizer tokens(tmp.front().c_str());
              tokens.parse();
              queue<group_token> parsed_token = tokens.cmd_args();
              rsh.exec_cmd(parsed_token);
              tmp.pop();
          }

      }
}
