/*#include<queue>
#include<vector>
#include<string>
void rsh(int);
void err_dump(const char *);
void write_sock(int, std::string);
void clear_queue(std::queue<std::vector<std::string>>&);
void parent_ps(int, int);
void child_ps(int , int , const char *,std::vector<char*> &, int ) ;
char *convert(const std::string & );*/
enum {
  NORMAL,
  PIPE_n,
  SUPERPIPE_n,
  PIPE_n_ERROR,
  SUPERPIPE_n_ERROR,
  WRITE_FILE,
  NORMAL_PIPE
};
