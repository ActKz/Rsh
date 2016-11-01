#pragma once
#include<string>
#include<queue>
#include<vector>
using namespace std;

class group_token{
public:
    int st;
    string file_name;
    vector<string> argv;
    int delay_pipe;
};

class Tokenizer{

public:
    Tokenizer(const char*);
    ~Tokenizer();

    queue<group_token> cmd_args();
    void parse();
    void clear_queue();
    void parse_single_command(char*);

private:
    char *_buf;
    queue<group_token> _cmd_args;

};
