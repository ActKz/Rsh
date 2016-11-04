#pragma once

using namespace std;

class Pipe{
    public:
        Pipe();
        ~Pipe();
        void close_read_pipe();
        void close_write_pipe();

        int pipes[2];
        int delay_pipe;
};
