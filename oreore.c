/*  -*- linux-c -*- */

#include "oreore.h"

/* flag to keep track of how we were registered */
static int secondary;
static struct dentry *dir_ino = NULL;
static struct dentry *config_ino = NULL;

extern struct file_operations oreore_file_ops;

static int max_priv_uid = 0;
module_param(max_priv_uid, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(max_priv_uid, "Debug enabled or not");

static int ignore_root;
module_param(ignore_root, bool, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(ignore_root, "Debug enabled or not");

static int debug;
module_param(debug, bool, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(debug, "Debug enabled or not");

#define root_dbg(fmt, arg...)					\
	do {							\
		if (debug)					\
			printk(KERN_DEBUG "%s: %s: " fmt ,	\
				MY_NAME , __FUNCTION__ , 	\
				## arg);			\
	} while (0)

static int oreore_bprm_check_security (struct linux_binprm *bprm)
{
	int err = 0;
	struct vfsmount *mnt;
	struct dentry *dentry;
	const char *p;
	char *tmp;
	//char *path = kzalloc(255, GFP_KERNEL); // todo
	
	if((0 < current->uid) && (current->uid <= max_priv_uid)) {
		printk(KERN_DEBUG "UID:%d max_priv_uid is %d. privuser\n",
		       current->uid, max_priv_uid);
		return 0;
	}
	
	if(ignore_root && current->uid == 0)
		return 0;
	
	if(bprm->filename[0] == '/') {
		if(!oreore_is_allowed_path(bprm->filename)) {
			printk(KERN_NOTICE
			       "UID:%d GID:%d '%s' is not allowed to execute\n",
			       bprm->e_uid, bprm->e_gid, bprm->filename);
			return -EPERM;
		}
		return 0;
	}

	tmp = (char *)__get_free_page(GFP_KERNEL);
	if(!tmp)
		return -ENOMEM;
	
	mnt = mntget(bprm->file->f_vfsmnt);
	dentry = dget(bprm->file->f_dentry);
	p = d_path(dentry, mnt, tmp, PAGE_SIZE);
	dput(dentry);
	mntput(mnt);
	if (IS_ERR(p)) {
		err = PTR_ERR(p);
		goto out;
	}

	if(!oreore_is_allowed_path(p)) {
		printk(KERN_NOTICE
		       "UID:%d GID:%d '%s' is not allowed to execute\n",
		       bprm->e_uid, bprm->e_gid, p);
		err = -EPERM;
		goto out;
	}
	return 0;

out:
	free_page((unsigned long)tmp);
	return err;
}

static struct security_operations oreore_security_ops = {
	.bprm_check_security =		oreore_bprm_check_security
};

static int oreore_create_securityfs(void)
{
	dir_ino = securityfs_create_dir("oreore", NULL);
	if(IS_ERR(dir_ino))
		return PTR_ERR(dir_ino);
	
	config_ino = securityfs_create_file("control",S_IRUGO,dir_ino, NULL,
					    &oreore_file_ops);
	if(IS_ERR(config_ino)) {
		securityfs_remove(dir_ino);
		return PTR_ERR(config_ino);
	}
	return 0;
}

//http://gitorious.org/usb/usb/commit/d93e4c940f51ae06b59c14523c4d55947f9597d6
static void oreore_remove_securityfs(void)
{
	if(!config_ino || IS_ERR(config_ino))
		return;
	if(!dir_ino || IS_ERR(dir_ino))
		return;

	//spin_lock(&config_ino->d_lock);
	//refcount = config_ino->;
	//spin_unlock(&config_ino->d_lock);
/* 	if(atomic_read(&dir_ino->d_count) > 2) { */
/* 		//goto out; */
/* 	if(atomic_read(&config_ino->d_count) > 1) */
/* 		printk(KERN_INFO "***********************\n"); */
	//goto out;

	// config_ino, dir_inoを掴んでいるプロセスを探してkillする??
	/* Buggy */
	securityfs_remove(config_ino);
	securityfs_remove(dir_ino);
}

static int oreore_register_security(void)
{
	/* register ourselves with the security framework */
	if (register_security (&oreore_security_ops)) {
		printk (KERN_INFO "Failure registering oreore module "
			"with the kernel\n");
		/* try registering with primary module */
		if (mod_reg_security (MY_NAME, &oreore_security_ops)) {
			printk (KERN_INFO "Failure registering oreore "
				" module with primary security module.\n");
			return -EINVAL;
		}
		secondary = 1;
	}
	return 0;
}

static void oreore_unregister_security(void)
{
	/* remove ourselves from the security framework */
	if (secondary) {
		if (mod_unreg_security (MY_NAME, &oreore_security_ops))
			printk (KERN_INFO "Failure unregistering oreore "
				" module with primary module.\n");
	} else { 
		if (unregister_security (&oreore_security_ops)) {
			printk (KERN_INFO "Failure unregistering oreore "
				"module with the kernel\n");
		}
	}
}

static int __init oreore_init (void)
{
	int retval = 0;
	
	retval = oreore_register_security();
	if(retval)
		return retval;
	
	retval = oreore_create_securityfs();
	if(retval) {
		oreore_unregister_security();
		return retval;
	}

	oreore_setup();
	printk (KERN_INFO "oreore module registerd\n");
	return retval;
}

static void __exit oreore_exit (void)
{
	oreore_remove_securityfs();
	oreore_unregister_security();
	oreore_teardown();
	printk (KERN_INFO "oreore module removed\n");
}

security_initcall (oreore_init);
module_exit (oreore_exit);

MODULE_DESCRIPTION("Directory Limit ELF");
MODULE_LICENSE("GPL");
