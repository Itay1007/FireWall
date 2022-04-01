
#include "fw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;

/* packets between server and client need to  be drop */
static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	printk(KERN_INFO "hfuncInForward()\n");
	return NF_ACCEPT;
}

static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

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

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_module_init_function(void) {
	/* set the global struct pointer */
	nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);

	/* set the nfho fields */
	nfho->hook = (nf_hookfn*) hfuncInForward; /* set the hook function */ 
	nfho->hooknum = NF_INET_FORWARD; /* verdict packet in the prerouting state*/
	nfho->pf = PF_INET; /* for IPv4 */

	if(nf_register_net_hook(&init_net, nfho))
		return -1;

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

	return 0; /* if non-0 return means init_module failed */
}
 



/*
 * this function is called last when 
 * we remove the module from the kernel
 */
static void __exit my_module_exit_function(void) {

	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);

	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_life_my_rules.attr);
	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_my_world_inside.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "rules");
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);





