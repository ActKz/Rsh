#pragma once
#include <vector>

using namespace std;

class Pipe{
    public:
        Pipe(int);
        Pipe();
        ~Pipe();

        void rdr_input(int);
        void rdr_err();
        void rdr_output(int);
        void close_precmd();
        void close_nextcmd();
        int pipes[2];
};
