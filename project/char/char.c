#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>


static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;


static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class





static struct file_operations rules_fops = {
	.owner = THIS_MODULE
};

ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk(KERN_INFO "display()\n");
	return 0;
}

ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{	
	printk(KERN_INFO "modify()\n");
	return 0;
}

ssize_t another_display(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk(KERN_INFO "another_display()\n");
	return 0;
}

ssize_t another_modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{	
	printk(KERN_INFO "another_modify()\n");
	return 0;
}

static DEVICE_ATTR(my_life_my_rules, S_IWUSR | S_IRUGO , display, modify);
static DEVICE_ATTR(my_world_inside, S_IWUSR | S_IRUGO , another_display, another_modify);








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
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: read()\n");
    return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len,
    loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    return len;
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init ofcd_init(void) /* Constructor */
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "Namaskar: ofcd registered");


    	//create char device
	major_number = register_chrdev(0, "rules", &rules_fops);
	if (major_number < 0)
		return -1;
		
	//create sysfs class
	sysfs_class = class_create(THIS_MODULE, "fw");
	if (IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "rules");
		return -1;
	}
	
	//create sysfs device
	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "rules");	
	if (IS_ERR(sysfs_device))
	{
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "rules");
		return -1;
	}

	//create sysfs file attributes	
	if (device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_life_my_rules.attr))
	{
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "rules");
		return -1;
	}

	// create sysfs file attributes	
	if (device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_world_inside.attr))
	{
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "rules");
		return -1;
	}


    if ((ret = alloc_chrdev_region(&first, 0, 1, "Shweta")) < 0)
    {
        return ret;
    }
    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
    {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "mynull")))
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &pugs_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    return 0;
}

static void __exit ofcd_exit(void) /* Destructor */
{
	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_life_my_rules.attr);
	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_world_inside.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "rules");	

    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Alvida: ofcd unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia <email@sarika-pugs.com>");
MODULE_DESCRIPTION("Our First Character Driver");