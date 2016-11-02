#include "PipeClass.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


Pipe::Pipe(){
  if (pipe(pre_cmd) < 0 || pipe(next_cmd) < 0 || pipe(result) < 0)
      perror(strerror(errno));

}
Pipe::~Pipe(){
}

void Pipe::rdr2next(){
}
void Pipe::rdr_err(){
}
void Pipe::rdr_pre_cmd(){
}
