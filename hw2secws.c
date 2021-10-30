#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/fs.h>
#include <linux/device.h>


#define PACKET_ACCEPT_MSG "*** Packet Accepted ***"
#define PACKET_DROP_MSG  "*** Packet Dropped ***"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;

/* packets between server and client need to  be drop */
static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	printk(PACKET_DROP_MSG);
	return NF_DROP;
}


/* packets between server and FW or client and FW is acceped */
static unsigned int hfuncInInput(void *priv, struct sk_buff *skb,
			  const struct nf_hool_state *state)
{
	if(!skb)
		return NF_ACCEPT;

	printk(PACKET_ACCEPT_MSG);
	return NF_ACCEPT;
}


static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

static unsigned int sysfs_int = 0;

static struct file_operations fops = {
	.owner = THIS_MODULE
};

ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)	//sysfs show implementation
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", sysfs_int);
}

ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)	//sysfs store implementation
{
	int temp;
	if (sscanf(buf, "%u", &temp) == 1)
		sysfs_int = temp;
	return count;	
}

static DEVICE_ATTR(sysfs_att, S_IWUSR | S_IRUGO , display, modify);


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

	nf_register_net_hook(&init_net, nfho);


	nfho->hook = (nf_hookfn*) hfuncInInput;
	nfho->hooknum = NF_INET_LOCAL_IN;
	nf_register_net_hook(&init_net, nfho);

	
	//create char device
	major_number = register_chrdev(0, "Sysfs_Device", &fops);\
	if (major_number < 0)
		return -1;
		
	//create sysfs class
	sysfs_class = class_create(THIS_MODULE, "Sysfs_class");
	if (IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}
	
	//create sysfs device
	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "sysfs_class" "_" "sysfs_Device");	
	if (IS_ERR(sysfs_device))
	{
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}
	
	//create sysfs file attributes	
	if (device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_sysfs_att.attr))
	{
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "Sysfs_Device");
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


	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "Sysfs_Device");
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);





