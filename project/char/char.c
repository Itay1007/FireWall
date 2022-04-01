
#include "fw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");


struct cdev c_dev;
// static struct device *dev;


//---- Itay Barok Custom file operations of the log char device ----


int my_open(struct inode *_inode, struct file *_file)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

int my_close(struct inode *_inode, struct file *_file)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

ssize_t my_read(struct file *flip, char* buff, size_t length, loff_t *offp)
{
	printk(KERN_INFO "Driver: read()\n");
	return 0;
}

ssize_t my_write(struct file *flip, const char* buff, size_t length, loff_t *offp)
{
	printk(KERN_INFO "Driver: write()\n");
	return length;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
    .write = my_write
};

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_module_init_function(void) {

	int ret;
	struct device *dev_ret;


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

	return 0; /* if non 0 return means init_module failed */
}
 

/*
 * this function is called last when 
 * we remove the module from the kernel
 */
static void __exit my_module_exit_function(void) {
	cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
}

// call this init function when loading this kernel module 
module_init(my_module_init_function);
// call this exit function when unloading this kernel module 
module_exit(my_module_exit_function);
