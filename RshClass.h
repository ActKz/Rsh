#pragma once
#include <string>
#include <queue>
#include "TokenizerClass.h"
#include "PipeClass.h"
#define BUFF_SIZE 15000
using namespace std;
class Rsh{
public:
    Rsh(int);
    ~Rsh();
    void printenv(const string);
    void setenv(const string, const string);
    void chroot(const char*);
    void exec_cmd(queue<group_token>, vector<Pipe>&);
    void general_dup2(int,int);

    void prepare(const char*);
    void read_sock();
    void write_sock(string);
    void print_welcome();
    void print_prompt();
    char* buf();
    void buf_split();
    queue<string> readline();
    void clear_lines();
private:
    const char *_root = "ras";
    const string _welcome_str =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";
    int _sockfd;
    char _buf[BUFF_SIZE];
    int _buflen;
    queue<string> _lines;
    const char* _prompt= "% ";
};
