void cr0_write(unsigned long val);

int new_read(struct pt_regs *regs);

unsigned long unprotect_memory(void);

void protect_memory(unsigned long val);

void get_kallsyms_funcptr(void);

void get_syscall_table(void);

void hijack_read(void);
