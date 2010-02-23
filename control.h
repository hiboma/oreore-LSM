/*  -*- linux-c -*- */
#ifndef OREORE_FS_H
#define OREORE_FS_H 1

#include "oreore.h"
#include "ioctl.h"

void oreore_setup(void);
void oreore_teardown(void);
long ioctl_server_config(struct file *, unsigned int , unsigned long );
bool oreore_is_allowed_path(const char *);

/* ssize_t read_server_config(struct file *filp, */
/*                            char __user *buf, */
/*                            size_t count, */
/*                            loff_t *ppos); */

/* ssize_t write_server_config(struct file *filp, */
/*                             const char __user *ubuf, */
/*                             size_t length, loff_t *ppos); */


#endif
