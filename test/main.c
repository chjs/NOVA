#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define MAP_DEBUG	0x080000
#define MAP_AHEAD	0x100000
#define MAP_ATOMIC	0x200000

#define PAGE_SIZE	4096
#define PAGE_SHIFT	12
#define FILE_NAME	"/mnt/ramdisk/testfile"

int main (int argc, char *argv[]) {
	int i, j, fd, nr_pages;
	int flags = MAP_SHARED|MAP_DEBUG|MAP_ATOMIC;
	//int flags = MAP_SHARED|MAP_ATOMIC|MAP_DEBUG;
	//int flags = MAP_PRIVATE;
	//int flags = MAP_SHARED | MAP_DEBUG;
	struct stat file_info;
	void *mmap_buf;
	size_t mmap_size;
	char *source;
	char *copy_buf;
	int c = '0';

	printf("flags=%x\n", flags);

	/* allocate a buffer to copy */
	copy_buf = (char *)malloc (PAGE_SIZE);
	if (copy_buf == NULL) {
		printf("malloc failed\n");
		exit(1);
	}

	/* open a test file */
	fd = open(FILE_NAME, O_RDWR);
	if (fd == -1) {
		printf("open failed\n");
		exit(1);
	}

	/* get the test file information */
	if (0 > fstat (fd, &file_info)) {
		printf("stat failed\n");
		exit(1);
	}
	else {
		mmap_size = (size_t)file_info.st_size;
	}

        mmap_buf = mmap (0, mmap_size, PROT_READ|PROT_WRITE, flags, fd, 0);

	/*
	 * It checks the result of mmap.
	 */
	if (mmap_buf == MAP_FAILED) {
		printf("mmap failed\n");
		exit(1);
	}

	printf("mmap_buf=%p\n", mmap_buf);

	/* 같은 페이지 2번 읽기 */
	printf("1. read %.2048s\n", (char *)mmap_buf);		/* 2k read */
	printf("2. read %.2048s\n", (char *)mmap_buf + 2048);	/* 2k read */
	
	/* 첫번째 페이지에 100byte write 2번 */
	memset(copy_buf, ++c, PAGE_SIZE);			/* 1 */
	memcpy(mmap_buf, copy_buf, 100);			/* write */

	memset(copy_buf, ++c, PAGE_SIZE);			/* 2 */
	memcpy(mmap_buf+800, copy_buf, 100);			/* write */

	/* 첫번째 페이지를 한번 읽기 */
	printf("3. read %.4096s\n", (char *)mmap_buf);		/* 4k read */
	
	/* 세번째 페이지에 100byte write 2번 */
	memset(copy_buf, ++c, PAGE_SIZE);			/* 3 */
	memcpy(mmap_buf+(PAGE_SIZE*2), copy_buf, 100);		/* write */

	memset(copy_buf, ++c, PAGE_SIZE);			/* 4 */
	memcpy(mmap_buf+(PAGE_SIZE*2)+100, copy_buf, 100);		/* write */

	/* 다섯번째 페이지에 100byte write 1번 */
	memset(copy_buf, ++c, PAGE_SIZE);			/* 5 */
	memcpy(mmap_buf+(PAGE_SIZE*4), copy_buf, 100);		/* write */

	/* msync 호출 */
	msync(mmap_buf, mmap_size, MS_SYNC);

	/* msync 호출 후에, 첫번째 페이지에 다시 100byte write */
	memset(copy_buf, ++c, PAGE_SIZE);			/* 6 */
	memcpy(mmap_buf+2000, copy_buf, 100);			/* write */

	/* msync 호출 */
	msync(mmap_buf, mmap_size, MS_SYNC);

	/* msync 호출 후, 첫번째 페이지 read */
	printf("4. read %.4096s\n", (char *)mmap_buf);		/* 4k read */

	/*
	 * It finishes the work.
	 */
	munmap (mmap_buf, mmap_size);
	free (copy_buf);
	close (fd);

	return 0;
}
