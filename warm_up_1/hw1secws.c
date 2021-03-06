#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO and for the 						Macros */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#define PACKET_ACCEPT_MSG "*** Packet Accepted ***"
#define PACKET_DROP_MSG  "*** Packet Dropped ***"

MODULE_LICENSE("GPL");

/* create a global struct variable to use the hook of Netfilter*/
static struct nf_hook_ops *nfho = NULL;

/* packets between server and client need to  be drop */
static unsigned int hfuncInForward(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	printk(PACKET_DROP_MSG);
	return NF_DROP;
}


static unsigned int hfuncInInput(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	if(!skb)
		return NF_ACCEPT;

	printk(PACKET_ACCEPT_MSG);
	return NF_ACCEPT;
}


static unsigned int hfuncInLocalOut(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *state)
{
	if(!skb)
		return NF_ACCEPT;

	printk(PACKET_ACCEPT_MSG);
	return NF_ACCEPT;
}


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

	nfho->hook = (nf_hookfn*) hfuncInLocalOut;
	nfho->hooknum = NF_INET_LOCAL_OUT;
	nf_register_net_hook(&init_net, nfho);

	return 0; /* if non-0 return means init_module failed */
}
 
/*
 * this function is called last when 
 * we remove the module from the kernel
 */
static void __exit my_module_exit_function(void) {

	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);
}

module_init(my_module_init_function);
module_exit(my_module_exit_function);


