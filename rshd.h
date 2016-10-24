#include <queue>
#include <tuple>
#include <vector>
#include <string>
void rsh(int);
void err_dump(const char *);
void write_sock(int, std::string);
enum {
  NORMAL,
  PIPE_n,
  SUPERPIPE_n,
  PIPE_n_ERROR,
  SUPERPIPE_n_ERROR,
  WRITE_FILE
};
