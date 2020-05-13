/* **************** LDD:2.0 s_20/lab_one_interrupt.h **************** */
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
#ifndef _LAB_ONE_INTERRUPT_H
#define _LAB_ONE INTERRUPT_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

/* everything we need for dynamic character device allocation */


#define MYIOC_TYPE 'k'
#define SHARED_IRQ 19
static int irq = SHARED_IRQ;
module_param(irq, int, S_IRUGO);

/* default delay time in top half -- try 10 to get results */
static int delay = 0;
module_param(delay, int, S_IRUGO);

static atomic_t counter_bh, counter_th;
static int START = 0, N = 1; //5-1
static unsigned long j_pre, j_interval, j_cur, avg_interval, min_interval, max_interval, t1, t2;
struct my_dat {
	unsigned long jiffies;	/* used for timestamp */
	struct tasklet_struct tsk;	/* used in dynamic tasklet solution */
	struct work_struct work;	/* used in dynamic workqueue solution */
};
static struct my_dat my_data;

#define MYDEV_NAME "mycdrv"
static struct class *foo_class;
static char *ramdisk;
static size_t ramdisk_size = (4 * PAGE_SIZE);
static dev_t first;
static unsigned int count = 1;  /* number of dev_t needed */
static struct cdev *my_cdev;
static const struct file_operations mycdrv_fops;

static irqreturn_t my_interrupt(int irq, void *dev_id);
#ifdef THREADED_IRQ
static irqreturn_t thread_fun(int irq, void *thr_arg);
#endif

static inline int mycdrv_generic_open (struct inode *inode, struct file *file)
{
    static int counter = 0;
    printk (KERN_INFO " attempting to open device: %s:\n", MYDEV_NAME);
    printk (KERN_INFO " MAJOR number = %d, MINOR number = %d\n",
            imajor (inode), iminor (inode));
    counter++;

    printk (KERN_INFO " successfully open  device: %s:\n\n", MYDEV_NAME);
    printk (KERN_INFO "I have been opened  %d times since being loaded\n",
            counter);
    printk (KERN_INFO "ref=%ld\n", module_refcount (THIS_MODULE));

    return 0;
}
static inline int mycdrv_generic_release (struct inode *inode,
                                          struct file *file)
{
    printk (KERN_INFO " closing character device: %s:\n\n", MYDEV_NAME);
    return 0;
}

static int __init my_generic_init(void)
{
	/* set irq request */
	atomic_set(&counter_bh, 0);
	atomic_set(&counter_th, 0);

	/* use my_data for dev_id */

#ifdef THREADED_IRQ
	if (request_threaded_irq(irq, my_interrupt, thread_fun, IRQF_SHARED,
				 "my_int", &my_data))
#else
	if (request_irq(irq, my_interrupt, IRQF_SHARED, "my_int", &my_data))
#endif
	{
		pr_warning("Failed to reserve irq %d\n", irq);
		return -1;
	}
	pr_info("successfully loaded\n");

	/* register device driver : mycdrv */
	if (alloc_chrdev_region (&first, 0, count, MYDEV_NAME) < 0) {
        printk (KERN_ERR "failed to allocate character device region\n");
        return -1;
    }
    if (!(my_cdev = cdev_alloc ())) {
        printk (KERN_ERR "cdev_alloc() failed\n");
        unregister_chrdev_region (first, count);
        return -1;
    }
    cdev_init (my_cdev, &mycdrv_fops);

    ramdisk = kmalloc (ramdisk_size, GFP_KERNEL);

    if (cdev_add (my_cdev, first, count) < 0) {
        printk (KERN_ERR "cdev_add() failed\n");
        cdev_del (my_cdev);
        unregister_chrdev_region (first, count);
        kfree (ramdisk);
        return -1;
    }
	foo_class = class_create (THIS_MODULE, "my_class");
    device_create (foo_class, NULL, first, "%s", "mycdrv");

    printk (KERN_INFO "\nSucceeded in registering character device %s\n",
            MYDEV_NAME);
    printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (first),
            MINOR (first));
	return 0;
}

static void __exit my_generic_exit(void)
{
	synchronize_irq(irq);
	free_irq(irq, &my_data);
	pr_info(" counter_th = %d,  counter_bh = %d\n",
		atomic_read(&counter_th), atomic_read(&counter_bh));
	pr_info("successfully unloaded\n");

	/* deregister device driver */

	device_destroy (foo_class, first);
    class_destroy (foo_class);
    if (my_cdev)
        cdev_del (my_cdev);
    unregister_chrdev_region (first, count);
    kfree (ramdisk);
    printk (KERN_INFO "\ndevice unregistered\n");
}

static inline long
mycdrv_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	
	int rc = 0, direction;
	int size;
	void __user *ioargp = (void __user *)arg;
	/* make sure it is a valid command */
	if (_IOC_TYPE(cmd) != MYIOC_TYPE) {
		pr_warning(" got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}

	/* get the size of the buffer*/
	size = _IOC_SIZE(cmd);

	direction = _IOC_DIR(cmd);

	switch (direction) {

	case _IOC_WRITE:
		
		pr_info("_IOC_WRITE\n");
		START = 1;
		break;

	case _IOC_READ:
		pr_info("START in ioc read : %d\n", START);
		rc = copy_to_user(ioargp, &START, size);
		break;

	default:
		pr_warning(" got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}

	return rc;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release
};

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LDD:2.0 s_20/lab_one_interrupt.h");
MODULE_LICENSE("GPL v2");

#endif
