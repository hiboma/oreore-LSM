/*  -*- linux-c -*- */

#ifndef DIR_LIMIT_ELF_IO_CTRL_H
#define DIR_LIMIT_ELF_IO_CTRL_H

#define IOC_MAGIC 'd'
#define IOCTL_VALSET    _IOW(IOC_MAGIC, 1,  struct ioctl_cmd)
#define IOCTL_VALGET    _IOWR(IOC_MAGIC, 2, struct ioctl_cmd)
#define IOCTL_VALDELETE _IOW(IOC_MAGIC, 3,  struct ioctl_cmd)

struct ioctl_cmd {
	unsigned int reg;
	unsigned int offset;
	unsigned int length;
	const char *path;
};

#endif
