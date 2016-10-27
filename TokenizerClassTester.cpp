#include "TokenizerClass.h"
#include <string>
#include <iostream>
using namespace std;
int main(){
    char *input;
    cin>>input;
    Tokenizer tmp(input);
    queue<struct GROUP_TOKEN> token = tmp.cmd_args();
    while(!token.empty()){

        token.pop();
    }
}
