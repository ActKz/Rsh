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

Tokenizer::Tokenizer(const char *input){
   int len = strlen(input);
   _buf = new char[len+1];
   memset(_buf,'\0',len+1);
   strcpy(_buf, input);
}

Tokenizer::~Tokenizer(){
//  _tokens->~group_token();
    while(!_cmd_args.empty()){
        _cmd_args.pop();
    }
    delete [] _buf;
//  _cmd_args->~queue();
}

queue<group_token> Tokenizer::cmd_args(){
    return _cmd_args;
}

void Tokenizer::parse(){
    char *saveptr = NULL;
    char *cptr = strtok_r(_buf, "|", &saveptr);
    while(cptr != NULL){
        cout<<cptr<<endl;
        parse_single_command(cptr);
        cptr = strtok_r(NULL, "|", &saveptr);
    }
//  _cptr = strtok(_buf, " ");
//  while (_cptr != NULL) {
//    if(_tokens->st == WRITE_FILE){
//        _tokens->file_name = strdup(_cptr);
//        break;
//    }
//    switch (_cptr[0]) {
//    case '|':
//      if (isdigit(_cptr[1])) {
//        _tokens->st = PIPE_n;
//        _delay_pipe = atoi(_cptr);
//      } else if (_cptr[1] == '\0'){
//        _tokens->st = NORMAL_PIPE;
//      }
//      else {
//        _tokens->st = PIPE_n_ERROR;
//      }
//      _cmd_args.push(*_tokens);
//      _tokens = new group_token;
//      break;
//    case '!':
//      if (isdigit(_cptr[1])) {
//        _tokens->st = SUPERPIPE_n;
//        _delay_pipe = atoi(_cptr);
//      } else{
//        _tokens->st = SUPERPIPE_n_ERROR;
//      }
//      _cmd_args.push(*_tokens);
//      break;
//    case '>':
//      _tokens->st = WRITE_FILE;
//      break;
//    default:
//      _tokens->argv.push_back(_cptr);
//    }
//    _cptr = strtok(NULL, " ");
//  }
//  _cmd_args.push(*_tokens);
}

void Tokenizer::parse_single_command(char *str){
    if(isdigit(str[0])){
        _cmd_args.back().st = PIPE_n;
        _cmd_args.back().delay_pipe = atoi(str);
        return;
    } else if(str[0]!=' ' && _cmd_args.size()!=0){
        _cmd_args.back().st = PIPE_n_ERROR;
        return;
    }
    char *saveptr = NULL;
    char *ptr = strtok_r(str," ", &saveptr);
    group_token tmp;
    tmp.st = NORMAL;
    while(ptr != NULL){
        switch(ptr[0]){
            case '!':
                if(ptr[1]=='\0' || !isdigit(ptr[1]))
                    tmp.st = SUPERPIPE_n_ERROR;
                else{
                    tmp.delay_pipe = atoi(&ptr[1]);
                    tmp.st = SUPERPIPE_n;
                }
                break;
            case '>':
                tmp.st = WRITE_FILE;
                break;
            default:
                if(tmp.st != WRITE_FILE){
                    string temp = ptr;
                    tmp.argv.push_back(temp);
                }
                else
                    tmp.file_name = ptr;
        }
        ptr = strtok_r(NULL, " ", &saveptr);
    }
    _cmd_args.push(tmp);
}


void Tokenizer::clear_queue(){
    while(!_cmd_args.empty())
        _cmd_args.pop();
}

#ifdef test
int main(){
    char input[15000]="a b c d | e f g h | i  !15";
//  fgets(input,15000,stdin);
    Tokenizer tmp(input);
    tmp.parse();
    queue<group_token> token = tmp.cmd_args();
    while(!token.empty()){
        cout<<"st: "<<token.front().st<<", argv: ";
        for(int i=0;i<token.front().argv.size();i++)
            cout<<token.front().argv[i]<<" ";
        cout<<", argv size: "<<token.front().argv.size()<<", write to: "<<token.front().file_name;
        cout<<endl;
        token.pop();
    }
    cout<<"END"<<endl;
}
#endif
