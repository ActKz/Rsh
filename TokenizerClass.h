#include <string>
#include <queue>
#include <vector>
using namespace std;

struct GROUP_TOKEN;

class Tokenizer{

public:
    Tokenizer(char*);
    ~Tokenizer();

    queue<struct GROUP_TOKEN> cmd_args();
    void parse();

private:
    int _delay_pipe;
    char *_cptr, *_buf;
    struct GROUP_TOKEN *_tokens;
    queue<struct GROUP_TOKEN> _cmd_args;

};
