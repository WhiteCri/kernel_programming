#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#define DEVICE_NAME	"/dev/kbd_led_drv"
#define BUF_LEN 2

void clearInput(){char c; while((c = getchar()) != '\n' && c != EOF) {}}
int main(int argc, char* argv[])
{
	int dev;
	char buf[BUF_LEN] = "";
	char ch;

	dev = open(DEVICE_NAME, O_RDWR | O_NDELAY);
	
	if(dev < 0)
	{
		printf("fail to device file open\n");
		return 0;
	}

	while(1)
	{
		printf("Input argument(| 0 | 1 | f | + | - |) : ");
		ch = getchar();
		int ok = 1;
		switch(ch)
		{
		case '0': printf("LED OFF\n"); break;
		case '1': printf("LED ON\n"); break;
		case 'f': 
		case 'F': printf("LED BLINK\n"); break;
		case '+': printf("BLINK DELAY UP\n"); break;
		case '-': printf("BLINK DELAY DOWN\n"); break;
		default : ok = 0; printf("INVALID ARGUMENT\n"); break;
		}
		clearInput();
		//check valid input
		if (0 == ok) continue;
		
		//write to device
		buf[0] = ch;
		buf[1] = '\0';
		write(dev, buf, BUF_LEN);
		
		//check if user want to exit
		int exit_program = 0;
		while(1){
			printf("Do you want to exit ? (Y / N) ");
			ch = getchar();
			clearInput();
			
			if (ch=='Y'||ch=='y') {
				exit_program = 20160371;
				break;
			}
			if (ch=='N'||ch=='n') break;
		}
		if (exit_program) break;
	}
	close(dev);
}
