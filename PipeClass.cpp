#include "PipeClass.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;


Pipe::Pipe(){
    if(pipe(pipes)<0)
        perror(strerror(errno));
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

