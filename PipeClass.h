class Pipe{
    public:
        Pipe();
        ~Pipe();

        void rdr2next();
        void rdr_err();
        void rdr_pre_cmd();
    private:
        int pre_cmd[2], next_cmd[2], result[2];
};
