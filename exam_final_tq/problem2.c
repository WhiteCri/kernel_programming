#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/jiffies.h>

#define DEVICE_NAME	"kbd_led_drv"
#define LED_SCROLL_LOCK	0x01
#define LED_NUM_LOCK	0x02
#define LED_CAPS_LOCK	0x04
#define LED_RESTORE	0xFF

struct tty_driver *kbd_drv;
int KBD_LED_MAJOR = 0;
int FLASH_TIME = HZ;
static void led_flash(unsigned long dummy);
static struct timer_list led_timer = TIMER_INITIALIZER(&led_flash, 0, 0);
//add timer with call_back, expires, data
static int led_state = 0;
static int is_flash = 0;
/* tw variables */
static char kbuf[2];

/* tw variables and */

static void led_flash(unsigned long dummy)
{
	if (0 == is_flash) return;
	pr_info("Flash!\n");
	led_state = !led_state;
	
	if (led_state)
		( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_CAPS_LOCK);
	else	
		( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_RESTORE); 

	mod_timer(&led_timer, FLASH_TIME + jiffies);
}

int kbd_led_open(struct inode *inodep, struct file *filp)
{
	printk("<0> called kbd_led_open\n\n");
	printk("<0> Keyboard Driver Opened! LED RESET(All LED OFF)\n");
	( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_RESTORE); 
	return 0;
}

int kbd_led_release(struct inode *inodep, struct file *filp)
{
	( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_RESTORE); 
	is_flash = 0;
	pr_info("release...\n");
	return 0;
}

static int kbd_led_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	printk("<0> called kbd_led_write\n\n");
	
	if( (buf==NULL) || (count<0) )
		return -EINVAL;

	//copy from user
	copy_from_user(kbuf, buf, count);

	printk("<0> The User's buf : %s\n\n", kbuf);
			
	switch(kbuf[0]) // Implement here
	{
		case '0': // LED OFF
			( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_RESTORE);
			is_flash = 0;
			led_state = 0;
			pr_info("led off....\n");
		break; 
		case '1': // LED ON
			( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_CAPS_LOCK);
			pr_info("led on....\n");
			is_flash = 0;
			led_state = 1;
			break; 
		case 'f': // LED BLINK
		case 'F':
			is_flash = 20160371;
			FLASH_TIME = HZ;
			led_flash(20160371);
			break;
		case '+': // LED BLINK DELAY PLUS
			FLASH_TIME *= 2; 
			break;
		case '-': // LED BLINK DELAY MINUS
			FLASH_TIME = (FLASH_TIME/2) > HZ ? FLASH_TIME/2 : HZ;
			break;
		default :
			printk("<0> Usage is Error\n");
			return -EINVAL;
	}
	return 0;
}

static inline long
mycdrv_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	
	pr_info("pleazzz\n");
	/* for r cmd */
	// send period to user
	int tmp = 10;
	int idx = 0;
	char digit_buf[64] = "";
	//get the number of digit of LED_FLASH
	int digit = 1;
	/* for r cmd end*/

	int i, rc, direction;
	int size;
	char *buffer;
	void __user *ioargp = (void __user *)arg;

	/* get the size of the buffer and kmalloc it */
	size = _IOC_SIZE(cmd);
	buffer = kmalloc((size_t) size, GFP_KERNEL);//dynamical memory 
	//allocation
	if (!buffer) {
		pr_err("Kmalloc failed for buffer\n");
		return -ENOMEM;
	}

	direction = _IOC_DIR(cmd);

	


	switch (direction) {
	case _IOC_WRITE:
		pr_info("_IOC_WRITE...shit...\n");
		rc = copy_from_user(buffer, ioargp, size);

		printk("<0> The User's buf : %s\n\n", buffer);
		break;
	case _IOC_READ:
		printk("<0> called kbd_led_read\n\n");

		copy_from_user(buffer, ioargp, size);

		printk("<0> The User's buf : %s\n\n", buffer);
				
		switch(buffer[0]) // Implement here
		{
			case '0': // LED OFF
				( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_RESTORE);
				is_flash = 0;
				led_state = 0;
				pr_info("led off....\n");
			break; 
			case '1': // LED ON
				( kbd_drv->ops->ioctl ) (vc_cons[fg_console].d->port.tty, KDSETLED,LED_CAPS_LOCK);
				pr_info("led on....\n");
				is_flash = 0;
				led_state = 1;
				break; 
			case 'f': // LED BLINK
			case 'F':
				is_flash = 20160371;
				FLASH_TIME = HZ;
				led_flash(20160371);
				break;
			case 'r': 
				pr_info("fuck you\n");
				pr_info("FLASH_TIME : %d, %x\n", FLASH_TIME, FLASH_TIME);
				tmp = 1;
				digit = 0;
				idx = 0;
				while (FLASH_TIME / tmp){
					digit_buf[idx++] = FLASH_TIME % (tmp * 10)/tmp;
					tmp *= 10;
					digit++;
					pr_info("digit_buf : %d\n");
				}
				
				//unsigned long to char *
				for(i = 0 ; i < digit; ++i)
					buffer[digit - 1 - i] = digit_buf[i] + '0';
				copy_to_user(ioargp, buffer, digit);
				pr_info("fuck you : %s\n");
				
				break;
			default :
				printk("<0> Usage is Error\n");
				return -EINVAL;
		}
		break;
	default:
		pr_warning(" got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}
	return rc;
}

struct file_operations kbd_drv_fops = {
	.owner = THIS_MODULE,
	.open = kbd_led_open,
	.release = kbd_led_release,
	.write = kbd_led_write,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
};

int kbd_led_init(void)
{
	int i;
	printk("<0> kbd_led Module is Loading\n");

	if( (KBD_LED_MAJOR = register_chrdev(0, DEVICE_NAME, &kbd_drv_fops)) < 0){
		printk("<0> Can't be registered\n");
		return KBD_LED_MAJOR;
	}
	printk("<0> Major No is %2d\n",KBD_LED_MAJOR);
	printk("<0> Make Character Device /dev/kbd_led_drv by mknod\n");
	printk("<0> mknod /dev/kbd_led_drv c %d 0\n",KBD_LED_MAJOR);
	
	// get kbd_drv in vc_cons using foreground console
	for(i = 0; i < MAX_NR_CONSOLES; i++){
		if( !vc_cons[i].d )
			break;
	printk("<0> console[%i/%i] #%i, tty %lx\n", i,
                       MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                       (unsigned long)vc_cons[i].d->port.tty);	
	}
	kbd_drv = vc_cons[fg_console].d->port.tty->driver;

	return 0;
}

void kbd_led_exit(void)
{
	del_timer(&led_timer);
	printk("<0> kbd_led Module is Unloading\n");
	printk("<0> Remove Character Device\n");
	printk("<0> rm /dev/kbd_led_drv\n");
}

module_init(kbd_led_init);
module_exit(kbd_led_exit);

MODULE_LICENSE("GPL");
