// #include <linux/kernel.h>
// #include <linux/module.h>
// #include <linux/netfilter.h>
// #include <linux/netfilter_ipv4.h>
// #include <linux/fs.h>
// #include <linux/device.h>
// #include <linux/uaccess.h>

#define PACKET_ACCEPT_MSG "*** Packet Accepted ***"
#define PACKET_DROP_MSG  "*** Packet Dropped ***"

#include "fw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;
static unsigned int packets_accept_number = 0;
static unsigned int packets_drop_number = 0;

/* packets between server and client need to  be drop */
static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	packets_drop_number++;

	printk(PACKET_DROP_MSG);
	return NF_DROP;
}

static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

static struct file_operations fops = {
	.owner = THIS_MODULE
};

ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)	//sysfs show implementation
{
	return scnprintf(buf, PAGE_SIZE, "%u\n%u\n", packets_accept_number, packets_drop_number); // set format and data in file
}

ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)	//sysfs store implementation
{	
	//cleanup of the packets values
	packets_accept_number = 0;
	packets_drop_number = 0;
	
	return count;	
}

static DEVICE_ATTR(rules, S_IWUSR | S_IRUGO , display, modify);

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
	major_number = register_chrdev(0, "fw", &fops);
	if (major_number < 0)
		return -1;
		
	//create sysfs class
	sysfs_class = class_create(THIS_MODULE, "fw");
	if (IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "fw");
		return -1;
	}
	
	//create sysfs device
	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "rules");	
	if (IS_ERR(sysfs_device))
	{
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "fw");
		return -1;
	}
	
	//create sysfs file attributes	
	if (device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr))
	{
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "fw");
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

	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "fw");
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);





