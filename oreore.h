#ifndef DIR_LIMIT_ELF_H
#define DIR_LIMIT_ELF_H 1

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/security.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <asm/uaccess.h>

#include "control.h"
#include "common.h"

#if defined(CONFIG_SECURITY_DIRLIMIT_ELF_MODULE)
#define MY_NAME THIS_MODULE->name
#else
#define MY_NAME "oreore"
#endif

struct oreore {
    struct list_head list;
    const char *path;
};

#endif
