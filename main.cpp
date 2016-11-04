#include "RshServer.h"
#include "RshClass.h"
#include "TokenizerClass.h"
#include "PipeClass.h"
#include <string>
#include <queue>
#include <vector>
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
      vector<Pipe> pipefd;
      for(;;){
          tmp=rsh.readline();
          while(!tmp.empty()){
              for(int i=0;i<pipefd.size();i++){
                  cout<<"i:"<<i<<";pipes: "<<pipefd[i].pipes[0]<<", "<<pipefd[i].pipes[1]<<";delay: "<<pipefd[i].delay_pipe<<endl;
//                if(pipefd[i].delay_pipe == 0){
//                  pipefd[i].close_read_pipe();
//                  pipefd.erase(pipefd.begin()+i);
//                  i--;
//                } else {
                    pipefd[i].delay_pipe -= 1;
//                }
              }
              Tokenizer tokens(tmp.front().c_str());
              tokens.parse();
              queue<group_token> parsed_token = tokens.cmd_args();
              rsh.exec_cmd(parsed_token, pipefd);
              tmp.pop();
          }

      }
}
