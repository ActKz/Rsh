#pragma once
#include<string>
#include<queue>
#include<vector>
using namespace std;

typedef struct GROUP_TOKEN{
    int st;
    string file_name;
    vector<string> argv;
}group_token;

class Tokenizer{

public:
    Tokenizer(const char*);
    ~Tokenizer();

    queue<group_token*> cmd_args();
    void parse();

private:
    int _delay_pipe;
    char *_cptr, *_buf;
    group_token *_tokens;
    queue<group_token*> _cmd_args;

};
