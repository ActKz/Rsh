#include "PipeClass.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


Pipe::Pipe(){
    if(pipe(pipes)<0)
        perror(strerror(errno));
}

Pipe::~Pipe(){

}

