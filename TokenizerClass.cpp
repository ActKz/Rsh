#include "TokenizerClass.h"
#include "rshd.h"
#include <string>
#include <queue>
#include <vector>
#include <cstdlib>

using namespace std;

typedef struct GROUP_TOKEN{
    int st;
    string *cmd;
    vector<string> argv;
}group_token;

Tokenizer::Tokenizer(char *input){
   _buf = input;
   _tokens = new group_token;
   _delay_pipe = 0;
}

queue<struct GROUP_TOKEN> Tokenizer::cmd_args(){
    return _cmd_args;
}

void Tokenizer::parse(){
    _cptr = strtok(_buf, " ");
    while (_cptr != NULL) {

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
        } else
          _tokens->st = SUPERPIPE_n_ERROR;
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
