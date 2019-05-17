#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
void lock_set(int fd, int type)
{
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	while (1){
		lock.l_type = type;
		if ((fcntl(fd, F_SETLK, &lock)) == 0){
			if (lock.l_type == F_RDLCK)
				printf("read lock set by %d\n", getpid());
			else if (lock.l_type == F_WRLCK)
				printf("write lock set by %d\n", getpid());
			else if (lock.l_type == F_UNLCK)
				printf("release lock by %d\n", getpid());
			else
				printf("what is be lock???\n");
			return;
		}
		/*
		 * 在实际的使用中遇到过问题
		 * 就是使用F_GETLK的时候,必须要保证l_type是已经被初始化了的
		 * 如果没有初始化就会导致l_type取出来的值是随机的值,根本达不到预期的目的
		 * */
		fcntl(fd, F_GETLK, &lock);
		if (lock.l_type != F_UNLCK){
			if (lock.l_type == F_RDLCK)
				printf("read lock already set by %d\n", lock.l_pid);
			else if (lock.l_type == F_WRLCK)
				printf("write lock already set by %d\n", lock.l_pid);
			else
				printf("why can't lock???\n");
			getchar();
		}
	}
}
int main(void)
{
	int fd;
	fd = open("hello", O_RDWR);
	if (fd < 0){
		perror("open");
		exit(1);
	}
	printf("fd >> %d\n", fd);
	lock_set(fd, F_WRLCK);
	getchar();
	lock_set(fd, F_UNLCK);
	getchar();
	close(fd);
	exit(0);
}
