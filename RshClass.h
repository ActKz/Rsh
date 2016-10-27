#include <string>
#include "TokenizerClass.h"
using namespace std;
class Rsh{
public:
    Rsh(int);
    ~Rsh();
    char* printenv(const char*);
    int setenv(const char*, const char*);
    void chroot(char*);
    void exec_cmd(queue<struct GROUP_TOKEN>);

    const char* welcome_str();
private:
    const char *_root = "ras";
    const string _welcome_str =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";
    int _sockfd;
};
