#ifndef H_READ_H
#define H_READ_H

int new_read(struct pt_regs *regs);
void hijack_read(void);
void restore_read(void);

#endif /* H_READ_H */