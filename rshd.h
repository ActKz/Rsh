#include <queue>
#include <tuple>
#include <vector>
#include <string>
void rsh(int);
void err_dump(const char *);
void cmd_parser(char*,std::queue<std::tuple<std::string, std::vector<std::string>>>*, int*,int* ,int);
