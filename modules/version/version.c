#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>

#include <linux/printk.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <asm/uaccess.h>
#include <asm/errno.h>

#include <linux/utsname.h>

#define VER_LENGTH		(65)

#define VERSION_MAGIC		'k'

#define VERSION_RESET		_IO(VERSION_MAGIC, 0)
#define VERSION_MODIFIED 	_IO(VERSION_MAGIC, 1)

static struct miscdevice version_md;
static struct file_operations version_fops;

char* ver = NULL;
int is_version_modified = 0;

void reset_default_ver(void) {
	size_t len;
	
	strncpy(ver, utsname()->release, strlen(utsname()->release));
	len = strlen(utsname()->release);
	ver[len] = '\n';
	ver[len + 1] = 0;
	is_version_modified = 0;
}

long int version_ioctl(struct file *fp, unsigned int ioctl_num, unsigned long ioctl_param) {
	
	int retval = 0;
	switch (ioctl_num) {	
	case VERSION_MODIFIED:
		printk(KERN_INFO "Went in VERSION_MODIFIED");
		retval = is_version_modified;
		break;
	case VERSION_RESET:
		printk(KERN_INFO "Went in VERSION_RESET");
		reset_default_ver();
		break;
	default:
		return -ENOTTY;
	}

	return retval;
}

static ssize_t version_read(struct file *fp, char *buffer, size_t length, loff_t *offset) 
{
	int bytes_read = 0;
	size_t len = 0;

	len = strlen(ver) + 1; 

	if (ver == 0)  return 0;

	while (ver[bytes_read] != 0) {
		put_user(ver[bytes_read], buffer += 1);
		bytes_read += 1;
	}
	put_user(0, &buffer[bytes_read]);
	bytes_read += 1;
	if (*offset == 0) {
		(*offset) = bytes_read;
		return bytes_read;
	}
	else return 0;
}

static ssize_t version_write(struct file *fp, const char *buffer, size_t length, loff_t *offset) 
{	
	/* The -2 in the condition is for the line feed and the terminating zero */
	int copy_length = length < VER_LENGTH - 2 ? length : VER_LENGTH;
	strncpy(ver, "", VER_LENGTH);
	if (copy_from_user(ver, buffer, copy_length)) {
		return -EFAULT;
	}
	is_version_modified = 1;
	return length;
}

static __init int version_init(void)
{
	int retval;	

	version_md.minor = MISC_DYNAMIC_MINOR;
	version_md.name = "version";
	version_md.fops = &version_fops;

	ver = kmalloc(VER_LENGTH * sizeof(char), GFP_KERNEL);
	version_fops.read = version_read; 
	version_fops.write = version_write;
	version_fops.unlocked_ioctl = version_ioctl;

	reset_default_ver();
	printk(KERN_INFO "Kernel version = %s", ver);
	
	retval = misc_register(&version_md);
	if (retval != 0) {
		return retval;
	}

	printk(KERN_INFO "Version miscdevice initialized, minor number = %d", version_md.minor);

	return 0;
}

static __exit void version_exit(void)
{	
	printk(KERN_INFO "Cleaning up");
	kfree(ver);
	misc_deregister(&version_md);
	return;
}

module_init(version_init);
module_exit(version_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module that creates a miscellaneous device that outputs the kernel version when probed");
MODULE_AUTHOR("Salim Dardouchi");
