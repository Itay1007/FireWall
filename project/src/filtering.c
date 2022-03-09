#include "fw.h"

#define ACCEPT 1
#define DROP 0


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

static struct file_operations fops = {
	.owner = THIS_MODULE
};

int verdict_paket_with_rule(void *priv, struct sk_buff *skb, const struct nf_hook_state *state, rule_t rule);
int verdict_packet(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);





static rule_t fw_table[MAX_RULES];
static int fw_free_ent_index = 0;

ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)	//sysfs show implementation
{
	return scnprintf(buf, PAGE_SIZE, "%s", str_of_table()); // set format and data in file
}

char * str_of_table()
{
	char *str = "";

	int i;
	for(i = 0; i < fw_free_ent_index; i++)
	{
		sprintf(str, "%s%s", str, str_of_table_ent(i));
	}
}

char * str_of_table_ent(int i)
{
	char *str = "";
	rule_t rule = fw_table[i];
	sprintf(str, "%s %s %s/%s %s/%s %s %s %s %s %s\n", 
				rule.rule_name,
				rule.direction,
				rule.src_ip,
				rule.src_prefix_size,
				rule.dst_ip,
				rule.dst_prefix_size,
				rule.protocol,
				rule.src_port,
				rule.dst_port,
				rule.ack,
				rule.action
			);
}


ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)	//sysfs store implementation
{	
	// split the big string by entries of firewall table
	
	fw_free_ent_index = 0;
	int temp;
	char * str_fw_table;
	char * str_rule;
	char * str_rule_field;
	const char sep[3] = "\r\n"
	const char fields_sep[2] = " "
	sscanf(buf, "%s", &str_fw_table) // check for no errors with return value
	
	str_rule = strtok(str_fw_table, sep);

	while(str_rule != NULL)
	{
		str_rule_field = strtok(str_rule, fields_sep);

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].rule_name = str_rule_field;
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].direction = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].src_ip = str_rule_field;
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].src_prefix_size = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}


		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].dst_ip = str_rule_field;
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].dst_prefix_size = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].protocol = str_rule_field;
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].src_port = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].dst_port = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].ack = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		if(str_rule_field != NULL)
		{
			fw_table[fw_free_ent_index].action = str_rule_field;		
			str_rule_field = strtok(str_rule, NULL);
		}

		fw_free_ent_index++;
		str_rule = strtok(str_fw_table, NULL);
	}

	return count;	
}


int verdict_packet(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	int i;

	for(i = 0; i < fw_free_ent_index; i++)
	{
		if(verdict_paket_with_rule(priv, skb, state, fw_table[i]) == ACCEPT)
		{
			return ACCEPT;
		}
	}

	return DROP;
}

int verdict_paket_with_rule(void *priv, struct sk_buff *skb, const struct nf_hook_state *state, rule_t rule)
{
	/*
	rule.rule_name, // seems not rellevant ?
	rule.direction,
	rule.src_ip,
	rule.src_prefix_size,
	rule.dst_ip,
	rule.dst_prefix_size,
	rule.protocol,
	rule.src_port,
	rule.dst_port,
	rule.ack, // TODO: what is it ?
	rule.action
	*/
}



static DEVICE_ATTR(sysfs_att, S_IWUSR | S_IRUGO , display, modify);

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_fw_table_init_function(void) {
	// path = "/sys/class/Sysfs_class/sysfs_class_sysfs_Device/sysfs_att"

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
 

/*
 * this function is called last when 
 * we remove the module from the kernel
 */
static void __exit my_fw_table_exit_function(void) {

	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);


	device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "Sysfs_Device");
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);





