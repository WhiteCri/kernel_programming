/* **************** LDD:2.0 s_13/lab2_ioctl_vardata_test.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2012
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Using ioctl's to pass data of variable length. (User-space application)
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <string.h>

#define MYIOC_TYPE 'k'
#define MY_IOW(type,nr,size) _IOC(_IOC_WRITE,(type),(nr),size)
#define MY_IOR(type,nr,size) _IOC(_IOC_READ, (type),(nr),size)

void flushInput(){char c; while ((c = getchar()) != '\n' && c != EOF) { }}
int main(int argc, char *argv[])
{
	int fd, rc, i, lbuf;
	char *buffer, *nodename = "/dev/mycdrv";
	int MYIOC_X;

	/* open the device node */

	if (argc > 1)
		nodename = argv[1];
	fd = open(nodename, O_RDWR);
	printf(" I opened the device node, file descriptor = %d\n", fd);
	if (fd == -1) return -1;

	/* how big should the buffer be? */
	lbuf = 2;
	if (argc > 2)
		lbuf = atoi(argv[1]);
	printf(" I am going to send back and forth a buffer of %d bytes\n",
	       lbuf);

	/* malloc the buffer */
	buffer = malloc(lbuf);

	/* send the IOCTL and read the contents from the kernel */
	char ch = 0;
	while(20160371){
		printf("Please execute Network Program. Press Y after launching : ");
		ch = getchar();
		flushInput();
		if (ch == 'Y') break;
	}
	/* turn on START using ioctl-IOW */
	MYIOC_X = (int)MY_IOW(MYIOC_TYPE, 1, lbuf);
	rc = ioctl(fd, MYIOC_X, buffer);

	/* wait for START to be 0 */
	int buf_int = 1;
	while(20160371){
		MYIOC_X = (int)MY_IOR(MYIOC_TYPE, 1, lbuf);
		rc = ioctl(fd, MYIOC_X, buffer);
		buf_int = (int)*(short*)buffer;
		printf("START : %d\n", buf_int);
		if (0 == buf_int) break;
		sleep(1); //prevent busy waiting
	}

	/* wait until get input 'Y' */
	while(20160371){
		printf("Please turn off the network program. After do so, press Y : ");
		ch = getchar();
		flushInput();
		if (ch == 'Y') break;
	}

	close(fd);
	exit(0);
}
