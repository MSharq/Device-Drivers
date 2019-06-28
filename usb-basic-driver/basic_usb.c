#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/usb.h>

#include<asm/uaccess.h> // copy_to_user, copy_from_user
#include<linux/uaccess.h>

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512

static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];

static int pen_open(struct inode *i, struct file *f)
{
	return 0;
}

static int pen_close(struct inode *i, struct file *f)
{
	return 0;
}

static ssize_t pen_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
	int retval;
	int read_cnt;

	/* Read the data from the bulk endpoint */
	retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
			bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);

	if (retval)
	{
		printk(KERN_ERR "Bulk message returned %d\n", retval);
		return retval;
	}

	if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
	{
		return -EFAULT;
	}

	return MIN(cnt, read_cnt);
}

static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
	int retval;
	int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

	if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
	{
		return -EFAULT;
	}

	/* Write the data into the bulk endpoint */
	retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
			bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);

	if (retval)
	{
		printk(KERN_ERR "Bulk message returned %d\n", retval);
		return retval;
	}

	return wrote_cnt;
}

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = pen_open,
	.release = pen_close,
	.read = pen_read,
	.write = pen_write,
};

static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{

	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i, retval;

	printk(KERN_INFO "pen drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);


	iface_desc = interface->cur_altsetting;
	printk(KERN_INFO "Pen i/f %d now probed: vendorId:productId (%04X:%04X)\n",
			iface_desc->desc.bInterfaceNumber,id->idVendor,id->idProduct);

	printk(KERN_INFO "ID->bNumEndpoints: %02X\n",iface_desc->desc.bNumEndpoints);
	printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
			iface_desc->desc.bInterfaceClass);

	for(i = 0; i < iface_desc->desc.bNumEndpoints; i++)
	{
		endpoint = &iface_desc->endpoint[i].desc;

		printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",i,
				endpoint->bEndpointAddress);
		printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",i,
				endpoint->bmAttributes);
		printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",i,
				endpoint->wMaxPacketSize,endpoint->wMaxPacketSize);
	}


	device = interface_to_usbdev(interface);

	class.name = "usb/pen%d";
	class.fops = &fops;
	if ((retval = usb_register_dev(interface, &class)) < 0)
	{
		/* something prevented us from registering this driver */
		printk(KERN_ERR "Not able to get a minor for this device.");
	}

	else
	{
		printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
	}

	return retval;
}

static void pen_disconnect(struct usb_interface *interface)
{
	usb_deregister_dev(interface, &class);
	printk(KERN_INFO "Pen i/f %d now disconnected\n",
			interface->cur_altsetting->desc.bInterfaceNumber);
}

static struct usb_device_id pen_table[] =
{
	{ USB_DEVICE(0x0781,0x5567) },
	{} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, pen_table);

static struct usb_driver pen_driver = 
{
	.name = "pen_driver_sharq",
	.id_table = pen_table,
	.probe = pen_probe,
	.disconnect = pen_disconnect,
};

static int pen_init(void)
{
	int result;

	/* Register this driver with the USB subsystem */
	if ((result = usb_register(&pen_driver)))
	{
		printk(KERN_ERR "usb_register failed. Error number %d", result);
	}

	return result;
}

static void pen_exit(void)
{
	/* Deregister this driver with the USB subsystem */
	usb_deregister(&pen_driver);
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sharq");
MODULE_DESCRIPTION("USB pen driver registration");
