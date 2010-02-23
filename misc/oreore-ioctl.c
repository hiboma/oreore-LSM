#include <sys/ioctl.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "ioctl.h"

static struct ioctl_cmd cmd;

void usage()
{
	fprintf(stderr, "usage: oreore-ioctl <path>\n");
	exit(1);
}

int main(int argc, const char *argv[]) {
	int ret = 0;
	int fd;
	
	fd = open("/sys/kernel/security/oreore/control",
		  O_NONBLOCK|O_WRONLY);
	if(fd < 0) {
		perror("open");
		return 1;
	}
	
	memset(&cmd, 0, sizeof(cmd));
	if(!strcmp(argv[1], "add")) {
		if(argc != 3) {
			usage();
		}
		cmd.path = argv[2];
		cmd.length = strlen(argv[2]);
		ret = ioctl(fd, IOCTL_VALSET, &cmd);
	}
	else if(!strcmp(argv[1], "delete")) {
		if(argc != 3) {
			usage();
		}
		cmd.path = argv[2];
		cmd.length = strlen(argv[2]);
		ret = ioctl(fd, IOCTL_VALDELETE, &cmd);
	}
	else if(!strcmp(argv[1], "get")) {
		cmd.offset = 0;
		cmd.path = malloc(255 * sizeof(char));
		int i =0;
		for(;;) {
			if((ret = ioctl(fd, IOCTL_VALGET, &cmd))) {
				if(errno == ENOENT) {
					exit(0);
				}
				perror("ioctl::get");
				exit(1);
			}
			printf("[%d] %s\n", cmd.offset, cmd.path);
			cmd.offset = ++i;
		}
	}
	else {
		usage();
	}
	
        if(ret == -1) {
		perror("ioctl:");
		return 1;
        }
	
        return 0;
}
