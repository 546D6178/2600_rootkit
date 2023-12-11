struct s_kill_args {
	char* action;
	int pid;
};
typedef struct s_kill_args t_kill_args;

struct s_hide_args {
	char *action;
	char *path;
};
typedef struct s_hide_args t_hide_args;

int parse_subcommand(const char* arg, const char **subcommands);
t_kill_args parse_kill_args(int argc, char **argv);
t_hide_args parse_hide_args(int argc, char **argv);
char* parse_read_args(int argc, char **argv);
int run(int argc, char **argv);
