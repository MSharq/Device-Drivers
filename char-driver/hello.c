#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h> // contains the cdev_init and cdev_add
#include<linux/device.h>

#include<asm/uaccess.h> // copy_to_user, copy_from_user
#include<linux/uaccess.h>

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static char c[5]; 

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

/* ssize_t means signed word, for "read" its the no of bytes read and for 
 * "write" it is the no of bytes read. Function "my_read" is invoked as 
 * a function request to read from the device driver, and return it to user.*/
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: read()\n");


	if (*off == 0)
	{
		if (copy_to_user(buf, &c, 3) != 0)
			return -EFAULT;
		else
		{
			(*off)++;
			return 1;
		}
	}
	else 
		return 0;

}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: write()\n");
	if (copy_from_user(&c, buf, 3) != 0) 
		return -EFAULT;
	else
	{
		printk(KERN_INFO "buf %s, len %ld, c %s...",buf,len,c);
		return len;
	}
}

static struct file_operations pugs_fops = 
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init ofcd_init(void)
{
	int ret;
	struct device *dev_ret;

	printk(KERN_INFO "Hello Sharq: registered");
	if ((ret = alloc_chrdev_region(&first, 0, 1, "Sharq")) < 0)
	{
		return ret;
	}

	printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));
	
	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(first, 1);
		return PTR_ERR(cl);
	}

	if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "yournull")))
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

	// initialize a cdev structure
	cdev_init(&c_dev, &pugs_fops);

	// add a char device to the system, adding it to the list of static struct kobj_map *cdev_map;
	if ((ret = cdev_add(&c_dev, first, 1)))
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return ret;
	}

	return 0;
}

static void __exit ofcd_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_EMERG "Bye Sharq: unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sharq");
MODULE_DESCRIPTION("Character Driver");
