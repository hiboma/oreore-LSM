#include "control.h"

const char *default_allowed_paths[] = {
	"/bin/",
	"/usr/bin/",
	"/usr/local/bin/",
	"/usr/lib/",
	"/usr/local/lib/",
	"/sbin/",
	"/usr/sbin/",
	"/usr/libexec/",
	"/etc/cron.daily/",
	NULL,
};

static struct rw_semaphore oreore_sem;
static struct list_head head;

void oreore_setup(void)
{
	init_rwsem(&oreore_sem);
	INIT_LIST_HEAD(&head);
}

void oreore_teardown(void)
{
	struct oreore *pos, *next;
	
	down_write(&oreore_sem);
	list_for_each_entry_safe(pos, next, &head, list) {
		list_del(&pos->list);
		oreore_free(pos);
	}
	up_write(&oreore_sem);
}

static bool oreore_statically_allowed(const char *path) {
	const char **allowed;

	for(allowed = default_allowed_paths; *allowed; allowed++) {
		if(oreore_strstarts(path, *allowed)) {
			return true;
		}
	}
	return false;
}

bool oreore_is_allowed_path(const char *path)
{
	int found = false;
	struct oreore *pos, *next;

	if(oreore_statically_allowed(path))
		return true;
		
	down_read(&oreore_sem);
	list_for_each_entry_safe(pos, next, &head, list) {
		if(oreore_strstarts(path, pos->path)) {
			found = true;
			break;
		}
	}
	up_read(&oreore_sem);
	return found;
}

int oreore_add(struct oreore *new)
{
	int retval = 0;
	struct oreore *pos, *next;

	down_write(&oreore_sem);
	list_for_each_entry_safe(pos, next, &head, list) {
		if(strcmp(pos->path, new->path) == 0) {
			retval = -EEXIST;
			goto out;
		}
	}
	list_add_tail(&new->list, &head);
out:
	up_write(&oreore_sem);
	return 0;
}

int oreore_delete(const char *path)
{
	int retval = -ENOENT;
	struct oreore *pos, *next;
	
	down_write(&oreore_sem);
	list_for_each_entry_safe(pos, next, &head, list) {
		if(strcmp(pos->path, path) == 0) {
			list_del(&pos->list);
			oreore_free(pos);
			retval = 0;
			break;
		}
	}
	up_write(&oreore_sem);
	return retval;
}

int oreore_get(struct ioctl_cmd *data)
{
	int i = 0;
	int retval = -ENOENT;
	struct oreore *pos ,*next;
	
	down_read(&oreore_sem);
	list_for_each_entry_safe(pos, next, &head, list) {
		if(data->offset == i) {
			if(copy_to_user((void __user *)data->path,
					pos->path, strlen(pos->path)+1)) {
				retval = -EFAULT;
				break;
			}
			data->offset = i;
			retval = 0;
			break;
		}
		i++;
	}
	up_read(&oreore_sem);
	return retval;
}

long oreore_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = -EFAULT;
	char *path = NULL;
	struct ioctl_cmd data;
	struct oreore *new;
	
	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;
	
	memset(&data, 0, sizeof(data));
	if(_IOC_DIR(cmd) & _IOC_READ) {
		
		if(!access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)))
			return -EFAULT;
		
		if(copy_from_user(&data, (void __user *)arg, sizeof(data)))
			return -EFAULT;
		
		path = getname(data.path);
		if(IS_ERR(path))
			return (PTR_ERR(path));
		putname(path);
		
	}
	else if (_IOC_DIR(cmd) & _IOC_WRITE) {
		if(!access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)))
			return -EFAULT;

		if(copy_from_user(&data, (void __user *)arg, sizeof(data)))
			return -EFAULT;

		if(!data.length)
			return -EFAULT;
		
		path = kzalloc((sizeof(char)*data.length)+2, GFP_KERNEL);
		if(!path)
			return -ENOMEM;
		
		if(strncpy_from_user(path, data.path, data.length) < 0)
			return -EFAULT;
	}
	else {
		return -EINVAL;
	}

	/* dispatch */
	switch(cmd) {
	case IOCTL_VALSET:
		
		retval = oreore_tidy_path(path);
		if(retval) {
			kfree(path);
			break;
		}
		new = oreore_alloc(path);
		if(!new) {
			kfree(path);
			retval = -ENOMEM;
			break;
		}
			
		retval = oreore_add(new);
		if(retval)
			kfree(path);
		break;
		
	case IOCTL_VALDELETE:
		retval = oreore_delete(path);
		kfree(path);
		break;
		
	case IOCTL_VALGET:
		
		retval = oreore_get(&data);
		if(retval)
			return retval;
		
		if(copy_to_user((void __user *)arg, &data, sizeof(data)))
			return -EFAULT;
		
		break;
	}

	return retval;
}

struct file_operations oreore_file_ops = {
/* 	.read  = read_server_config, */
/* 	.write = write_server_config, */
	.unlocked_ioctl = oreore_ioctl,
	.compat_ioctl   = oreore_ioctl,
};

/* ssize_t read_server_config(struct file *filp, */
/* 			   char __user *buf, */
/* 			   size_t count, */
/* 			   loff_t *ppos) */
/* { */
/* 	int i = 0; */
/* 	char tmp[256]; */
/* 	ssize_t len; */
/* 	struct oreore *pos, *next; */
	
/* 	if(!capable(CAP_SYS_ADMIN)) */
/* 		return -EPERM; */
	
/* 	down_read(&oreore_sem); */
/* 	list_for_each_entry_safe(pos, next, &head, list) { */
/* 		printk(KERN_INFO "[%d] regsiterd dirs is %s\n", i++, pos->path); */
/* 	} */
/* 	up_read(&oreore_sem); */
	
/* 	len = scnprintf(tmp, sizeof(tmp), "%s:%d\n", "192.168.0.1", 80); */
/* 	return simple_read_from_buffer(buf, count, ppos, tmp, len); */
/* } */

/* ssize_t write_server_config(struct file *filp, */
/* 			    const char __user *ubuf, */
/* 			    size_t length, */
/* 			    loff_t *ppos) */
/* { */
/* 	int err = 0; */
/* 	char *path; */
	
/* 	if(!capable(CAP_SYS_ADMIN)) */
/* 		return -EPERM; */
	
/* 	if(!access_ok(VERIFY_READ, (char __user *)ubuf, length)) */
/* 		return -EFAULT; */

/* 	path = kzalloc((sizeof(char)*length)+2, GFP_KERNEL); */
/* 	if(!path) */
/* 		return -ENOMEM; */

/* 	err = -EFAULT; */
/* 	if(strncpy_from_user(path, ubuf, length) < 0) */
/* 		goto error; */
	
/* /\* 	err = oreore_add_allowed_path(path); *\/ */
/* /\* 	if(err) *\/ */
/* /\* 		goto error; *\/ */
	
/* 	return length; */
	
/* error: */
/* 	kfree(path); */
/* 	return err; */
/* } */
/* 		path = getname(data.path); */
/* 		if(IS_ERR(path)) */
/* 			return (PTR_ERR(path)); */
/* 		putname(path); */
