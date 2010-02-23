#include "common.h"

/* static const char *oreore_mode_4[4] = { */
/* 	"disabled", "learning", "permissive", "enforcing" */
/* }; */

inline bool oreore_strstarts(const char *src, const char *find)
{
	const int len = strlen(find);
	if (strncmp(src, find, len))
		return false;
	
	return true;
}

struct oreore *oreore_alloc(const char *path)
{
	struct oreore *new;

	if(!path)
		return NULL;
	
	new = kzalloc(sizeof(struct oreore), GFP_KERNEL);
	if(!new)
		return NULL;
	
	new->path = path;
	return new;
}

void oreore_free(struct oreore *dir)
{
	if(!dir)
		return;
	if(dir->path)
		kfree(dir->path);
	kfree(dir);
}

int oreore_tidy_path(char *path)
{
	struct nameidata nd;
	char *found;
	int err = -EFAULT;
	int length;
	
	if(!path)
	  return -EFAULT;
	
	if(path[0] != '/')
		goto error;
	
	if((found = strchr(path, '\n')) != NULL)
		*found = '\0';
	
	length = strlen(path);
	if(path[length-1] != '/') {
	  strlcat(path, "/", length+1);
	}
	
	err = path_lookup(path, LOOKUP_DIRECTORY, &nd);
	if(err)
		goto error;
	
	err = -EINVAL;	
	if(!S_ISDIR(nd.dentry->d_inode->i_mode))
		goto error;
	
	return 0;
	
error:
	kfree(path);
	return err;
}


int oreore_dup_path(const char __user *filename, char **dest,
			   ssize_t length)
{
	struct nameidata nd;
	char *path, *found;
	int err = 0;

/* 	if(strncpy_from_user(path, filename, length) < 0) */
/* 	  return -EFAULT; */
	path = kzalloc(length + 1, GFP_KERNEL);
	if(!path)
		return -ENOMEM;
	
	err = -EFAULT;
	if(copy_from_user(path, filename, length))
		goto error;
	
	err = -EINVAL;	
	if(path[0] != '/')
		goto error;
	
	if((found = strchr(path, '\n')) != NULL) {
		*found = '\0';
	} else {
		path[length] = '\0';
	}
	
	if(path[length-1] != '/') {
		path[length]   = '/';
		path[length+1] = '\0';
	}
	
	err = path_lookup(path, LOOKUP_DIRECTORY, &nd);
	if(err)
		goto error;
	
	err = -EINVAL;	
	if(!S_ISDIR(nd.dentry->d_inode->i_mode))
		goto error;
	
	*dest = path;
	return 0;
	
error:
	kfree(path);
	return err;
}
