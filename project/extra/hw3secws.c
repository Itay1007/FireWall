
#include "fw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;


static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device_rules = NULL;
static struct device* sysfs_device_reset = NULL;

// static struct cdev c_dev;
// static struct device *dev;

// static unsigned int sysfs_int = 0;
// static unsigned int sysfs_int_2 = 1;
// static struct class *cl; // Global variable for the device class


// ---- Itay Barok Custom file operations of the log char device ----


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
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
    .write = my_write
};


// ---- Finished Itay Barok Custom file operations of the log char device ----


// ---- Itay Barok Custom display and modify sysfs char ---- 
// ----	of the rules char device 						----


ssize_t rules_display(struct device *dev, struct device_attribute *attr, char* buf)
{
	printk(KERN_INFO "rules_display()\n");
	return 0;

	// return scnprintf(buf, PAGE_SIZE, "%u\n", sysfs_int);
}

ssize_t rules_modify(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	printk(KERN_INFO "rules_modify()\n");
	return 0;
}

// connect the sysfs devices to their corresponding modify (write like)
// and display (read like) interface functions
static DEVICE_ATTR(sysfs_att, S_IWUSR | S_IRUGO , rules_display, rules_modify);


// ---- Finsihed Itay Barok Custom display and modify   ---- 
// ----	of the rules char device sysfs char  			----




// ---- Itay Barok Custom display and modify sysfs char ---- 
// ----	of the log reset char device 					----


ssize_t reset_display(struct device *dev, struct device_attribute *attr, char* buf)
{
	printk(KERN_INFO "rules_display()\n");
	return 0;
	// return scnprintf(buf, PAGE_SIZE, "%u\n", sysfs_int_2);
}

ssize_t reset_modify(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	printk(KERN_INFO "rules_display()\n");
	return 0;

	// return count;
}

// connect the sysfs devices to their corresponding modify (write like)
// and display (read like) interface functions
static DEVICE_ATTR(sysfs_att2, S_IWUSR | S_IRUGO , reset_display, reset_modify);


// ---- Finshed Itay Barok Custom display and modify sysfs char ---- 
// ----	of the log reset char device 							----




static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	return NF_ACCEPT;
}

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_module_init_function(void) {

	printk(KERN_INFO "in init()");
	// ---- init network on farward handing ----

	/* set the global struct pointer */
	if((nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL)) == NULL)
	{
		return -1;
	}

	/* set the nfho fields */
	nfho->hook = (nf_hookfn*) hfuncInForward; /* set the hook function */ 
	nfho->hooknum = NF_INET_FORWARD; /* verdict packet in the prerouting state*/
	nfho->pf = PF_INET; /* for IPv4 */

	if(nf_register_net_hook(&init_net, nfho) == -1)
	{
		return -1;
	}

	// ---- finished init on farward handing ----
	




	// ---- createing the log char device ----
	


	// ---- finished createing the log char device ----





	// ---- createing the rules sysfs device ----
	// ---- createing the reset sysfs device ----



	major_number = register_chrdev(0, "fw", &fops);

	if(major_number < 0)
	{
		return -1;
	}

	sysfs_class = class_create(THIS_MODULE, "fw");


	if(IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "fw");
		return -1;
	}

	sysfs_device_rules = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "rules");
	sysfs_device_reset = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "log");

	if(IS_ERR(sysfs_class))
	{
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "fw");
		return -1;
	}

	if(device_create_file(sysfs_device_rules, (const struct device_attribute *)&dev_attr_sysfs_att.attr))
	{
		device_remove_file(sysfs_device_rules, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "fw");
		return -1;
	}

	if(device_create_file(sysfs_device_reset, (const struct device_attribute *)&dev_attr_sysfs_att2.attr))
	{
		device_remove_file(sysfs_device_rules, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
		device_remove_file(sysfs_device_reset, (const struct device_attribute *)&dev_attr_sysfs_att2.attr);
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "fw");
		return -1;
	}
	

	// ---- finished createing the reset sysfs device ----
	// ---- finished createing the rulse sysfs device ----


	return 0; /* if non 0 return means init_module failed */
}
 

/*
 * this function is called last when 
 * we remove the module from the kernel
 */
static void __exit my_module_exit_function(void) {
	// delete the network handler resources
	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);

	// // delete the char device
	// cdev_del(&c_dev);
 //    device_destroy(cl, first);
 //    class_destroy(cl);
 //    unregister_chrdev_region(first, 1);
 //    printk(KERN_INFO "Alvida: ofcd unregistered");
	
	// delete the sysfs devices
	device_remove_file(sysfs_device_rules, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
	device_remove_file(sysfs_device_reset, (const struct device_attribute *)&dev_attr_sysfs_att2.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "fw");
}

// call this init function when loading this kernel module 
module_init(my_module_init_function);
// call this exit function when unloading this kernel module 
module_exit(my_module_exit_function);
