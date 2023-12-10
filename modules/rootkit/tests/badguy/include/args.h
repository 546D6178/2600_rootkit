struct s_kill_args {
	int pid;
	const char* action;
};
typedef struct s_kill_args t_kill_args;

int parse_subcommand(const char* arg);
t_kill_args parse_kill_args(int argc, char **argv);
char* parse_read_args(int argc, char **argv);
