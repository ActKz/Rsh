#include "PipeClass.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;


Pipe::Pipe(){
    if(pipe(pipes)<0)
        cerr << "Create Pipe fail: " << strerror(errno)<<endl;
}

Pipe::~Pipe(){
}

void Pipe::close_read_pipe(){
    if(close(pipes[0])!=0)
        cerr << "Close failed: pipefd="<<pipes[0]<<", "<<strerror(errno)<<endl;
}
void Pipe::close_write_pipe(){
    if(close(pipes[1])!=0)
        cerr << "Close failed: pipefd="<<pipes[1]<<", "<<strerror(errno)<<endl;
}
void Pipe::rdr_err2write(){
    int rett;
      rett = dup2(pipes[1], STDERR_FILENO);
      if (rett < 0)
        perror("DUP STDERR FAIL");
}
void Pipe::rdr_out2write(){
    int rett;
      rett = dup2(pipes[1], STDOUT_FILENO);
      if (rett < 0)
        perror("DUP STDOUT FAIL");
}
void Pipe::rdr_read2in(){
  int rett;
  rett = dup2(pipes[0], STDIN_FILENO);
  if (rett < 0)
    perror("DUP STDIN FAIL");
  else
    //                  close(pipefd[i].pipes[0]);
    close_read_pipe();
}

