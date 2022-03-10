#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/fs.h>
#include <linux/device.h>

#include "fw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Itay Barok");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;

// static unsigned int packets_accept_number = 0;
// static unsigned int packets_drop_number = 0;


static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	int valid = verdict_packet(priv, skb, state);
	
	if(valid) return NF_ACCEPT;
	
	//update_log_table(priv, skb, state);
	return NF_DROP;
}

/*
 * this initialization function is called first  
 * when we insert the module to the kernel
 */
static int __init my_module_init_function(void) {
	// cal for init of firewall module and init of log table

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

	my_filtering_table_init_function();
	//my_logs_table_init_function();

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

	my_filtering_table_exit_function();
	//my_logs_table_exit_function();
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);
