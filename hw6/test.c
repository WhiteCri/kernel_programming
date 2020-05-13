#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<curses.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#define DEVICE_NAME	"/dev/kbd_led_drv"

int main(int argc, char* argv[])
{
	int dev;
	char buf[2];
	char a;

	dev = open(DEVICE_NAME, O_RDWR | O_NDELAY);
	
	if(dev < 0)
	{
		printf("fail to device file open\n");
		return 0;
	}

	while(1)
	{
		//Implement here
	}
}
