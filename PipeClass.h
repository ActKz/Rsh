#pragma once

class Pipe{
    public:
        Pipe();
        ~Pipe();
        void close_read_pipe();
        void close_write_pipe();
        void rdr_err2write();
        void rdr_out2write();
        void rdr_read2in();


        int pipes[2];
        int delay_pipe;
};
