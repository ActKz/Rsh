#include "TokenizerClass.h"
#include "rshd.h"
#include <string>
#include <queue>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>

using namespace std;

typedef struct GROUP_TOKEN{
    int st;
    string file_name;
    vector<string> argv;
}group_token;

Tokenizer::Tokenizer(const char *input){
   _buf = strdup(input);
   _tokens = new group_token;
   _delay_pipe = 0;
}

Tokenizer::~Tokenizer(){
    _tokens->~GROUP_TOKEN();
}

queue<struct GROUP_TOKEN*> Tokenizer::cmd_args(){
    return _cmd_args;
}

void Tokenizer::parse(){
    _cptr = strtok(_buf, " ");
    while (_cptr != NULL) {
      if(_tokens->st == WRITE_FILE){
          _tokens->file_name = strdup(_cptr);
          break;
      }
      switch (_cptr[0]) {
      case '|':
        if (isdigit(_cptr[1])) {
          _tokens->st = PIPE_n;
          _delay_pipe = atoi(_cptr);
        } else if (_cptr[1] == '\0'){
          _tokens->st = NORMAL_PIPE;
        }
        else {
          _tokens->st = PIPE_n_ERROR;
        }
        _cmd_args.push(_tokens);
        _tokens = new group_token;
        break;
      case '!':
        if (isdigit(_cptr[1])) {
          _tokens->st = SUPERPIPE_n;
          _delay_pipe = atoi(_cptr);
        } else{
          _tokens->st = SUPERPIPE_n_ERROR;
        }
        _cmd_args.push(_tokens);
        break;
      case '>':
        _tokens->st = WRITE_FILE;
        break;
      default:
        _tokens->argv.push_back(_cptr);
      }
      _cptr = strtok(NULL, " ");
    }
    _cmd_args.push(_tokens);
}

#ifdef test
int main(){
    char input[15000];
    fgets(input,15000,stdin);
    Tokenizer tmp(input);
    tmp.parse();
    queue<struct GROUP_TOKEN*> token = tmp.cmd_args();
    while(!token.empty()){
        cout<<"st: "<<token.front()->st<<", argv: ";
        for(int i=0;i<token.front()->argv.size();i++)
            cout<<token.front()->argv[i]<<" ";
        cout<<", write to: "<<token.front()->file_name;
        cout<<endl;
        token.pop();
    }
    cout<<"END"<<endl;
}
#endif
