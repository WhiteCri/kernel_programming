#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>
#include <linux/delay.h>
#include <linux/timer.h>
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
static int led_state = 0;
static int is_flash = 0;

static void led_flash(unsigned long dummy)
{
	//Implement here
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
	return 0;
}

static int kbd_led_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int i = 0;
		
	printk("<0> called kbd_led_write\n\n");
	
	if( (buf==NULL) || (count<0) )
		return -EINVAL;

	printk("<0> The User's buf : %s\n\n", buf);
			
	switch(buf[i]) // Implement here
	{
		case '0': // LED OFF
		case '1': // LED ON
		case 'f': // LED BLINK
		case 'F':
		case '+': // LED BLINK DELAY PLUS
		case '-': // LED BLINK DELAY MINUS
		default :
			printk("<0> Usage is Error\n");
			return -EINVAL;
	}
	return 0;
}

struct file_operations kbd_drv_fops = {
	.owner = THIS_MODULE,
	.open = kbd_led_open,
	.release = kbd_led_release,
	.write = kbd_led_write,
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
