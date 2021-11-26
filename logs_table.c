#include "fw.h"
#include <linux/time.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

static struct file_operations fops = {
	.owner = THIS_MODULE
};


static rule_t fw_table[MAX_RULES];
static int fw_free_ent_index = 0;

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

static DEVICE_ATTR(sysfs_att, S_IWUSR | S_IRUGO , display, modify);

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_fw_table_init_function(void) {
	//"/sys/class/Sysfs_class/sysfs_class_sysfs_Device/sysfs_att"

	//create char device
	major_number = register_chrdev(0, "Sysfs_Device", &fops);
	if (major_number < 0)
		return -1;
		
	//create sysfs class
	sysfs_class = class_create(THIS_MODULE, "fw");
	if (IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}
	
	//create sysfs device
	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "rules");	
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
 


void on_packet(/* add 3 parameters here*/)
{
	struct log_rot_t log_entry;
	log_entry.timestamp = calc_time(); // time of creation/update
	log_entry.protocol = ;     	// values from: prot_t
	log_entry.action = ;       	// valid values: NF_ACCEPT, NF_DROP
	log_entry.src_ip = ;		  	// if you use this struct in userspace, change the type to unsigned int
	log_entry.dst_ip = ;		  	// if you use this struct in userspace, change the type to unsigned int
	log_entry.src_port;	  	// if you use this struct in userspace, change the type to unsigned short
	log_entry.dst_port;	  	// if you use this struct in userspace, change the type to unsigned short
	log_entry.reason;       	// rule#index, or values from: reason_t
	unsigned int   	count;        	// counts this line's hits
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





