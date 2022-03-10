// #include "fw.h"
// #include <linux/time.h>
// #include <linux/device.h>
// #include <linux/uaccess.h>

// MODULE_LICENSE("GPL");
// MODULE_AUTHOR("Itay Barok");

// static int major_number;
// static struct class* sysfs_class = NULL;
// static struct device* sysfs_device = NULL;

// static struct file_operations fops = {
// 	.owner = THIS_MODULE
// };

// int index;
// char* buffer_index;							// The moving index of the original buffer
// static int major_number;					// Major of the char device
// static struct class* sysfs_class = NULL;	// The device's class
// static struct device* sysfs_device = NULL;	// The device's name

// /* Our custom open function  for file_operations --------------------- */
// int my_open(struct inode *_inode, struct file *_file) { // Each time we open the device we initilize the changing variables ( so we will be able to read it again and again
// 	index = 0;
// 	str_len = strlen(test_string);
// 	buffer_index = test_string;
// 	return 0;
// }

// /* Our custom read function  for file_operations --------------------- */
// ssize_t my_read(struct file *filp, char *buff, size_t length, loff_t *offp) {
// 	ssize_t num_of_bytes;

// 	num_of_bytes = (str_len < length) ? str_len : length;
    
//     if (num_of_bytes == 0) { // We check to see if there's anything to write to the user
//     	return 0;
// 	}
    
//     if (copy_to_user(buff, buffer_index, num_of_bytes)) { // Send the data to the user through 'copy_to_user'
//         return -EFAULT;
//     } else { // fuction succeed, we just sent the user 'num_of_bytes' bytes, so we updating the counter and the string pointer index
//         str_len -= num_of_bytes;
//         buffer_index += num_of_bytes;
//         return num_of_bytes;
//     }
// 	return -EFAULT; // Should never reach here
// }

// static struct file_operations fops = { // Our 'file_operations' struct with declerations on our functions
// 	.owner = THIS_MODULE,
// 	.read = my_read,
// 	// .open = my_open,
// 	// .write = my_write,
// 	// .close = my_close
// };



// /*
//  * this initialization function is called first  
//  * when we insert the module to the kernel
//  */
// static int __init chardev_example_init(void) {
// 	major_number = register_chrdev(0, "Chardev_Device", &fops);\
	
// 	if (major_number < 0) {
// 		return -1;
// 	}
	
// 	sysfs_class = class_create(THIS_MODULE, "Sysfs_class");
	
// 	if (IS_ERR(sysfs_class)) {
// 		unregister_chrdev(major_number, "Sysfs_Device");
// 		return -1;
// 	}

// 	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "sysfs_class" "_" "sysfs_Device");	

// 	if (IS_ERR(sysfs_device)) {
// 		class_destroy(sysfs_class);
// 		unregister_chrdev(major_number, "Sysfs_Device");
// 		return -1;
// 	}	

// 	return 0;
// }

// /*
//  * this function is called last when 
//  * we remove the module from the kernel
//  */
// static void __exit my_logs_table_exit_function(void) {
// 	device_destroy(sysfs_class, MKDEV(major_number, 0));
// 	class_destroy(sysfs_class);
// 	unregister_chrdev(major_number, "Sysfs_Device");
// }


// void on_packet(/* add 3 parameters here*/)
// {
// 	struct log_rot_t log_entry;
// 	log_entry.timestamp = calc_time(); // time of creation/update
// 	log_entry.protocol = ;     	// values from: prot_t
// 	log_entry.action = ;       	// valid values: NF_ACCEPT, NF_DROP
// 	log_entry.src_ip = ;		  	// if you use this struct in userspace, change the type to unsigned int
// 	log_entry.dst_ip = ;		  	// if you use this struct in userspace, change the type to unsigned int
// 	log_entry.src_port;	  	// if you use this struct in userspace, change the type to unsigned short
// 	log_entry.dst_port;	  	// if you use this struct in userspace, change the type to unsigned short
// 	log_entry.reason;       	// rule#index, or values from: reason_t
// 	unsigned int   	count;        	// counts this line's hits
// }


// /*
//  * this initialization function is called first  
//  * when we insert the module to the kernel
//  */
// static int __init my_logs_table_init_function(void) {
// 	major_number = register_chrdev(0, "Chardev_Device", &fops);\
	
// 	if (major_number < 0) {
// 		return -1;
// 	}
	
// 	sysfs_class = class_create(THIS_MODULE, "Sysfs_class");
	
// 	if (IS_ERR(sysfs_class)) {
// 		unregister_chrdev(major_number, "Sysfs_Device");
// 		return -1;
// 	}

// 	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "sysfs_class" "_" "sysfs_Device");	

// 	if (IS_ERR(sysfs_device)) {
// 		class_destroy(sysfs_class);
// 		unregister_chrdev(major_number, "Sysfs_Device");
// 		return -1;
// 	}	

// 	return 0;
// }


// /*
//  * this function is called last when 
//  * we remove the module from the kernel
//  */
// static void __exit my_logs_table_exit_function(void) {

// 	device_destroy(sysfs_class, MKDEV(major_number, 0));
// 	class_destroy(sysfs_class);
// 	unregister_chrdev(major_number, "Sysfs_Device");
// }

// module_init(my_module_init_function);
// module_exit(my_logs_table_exit_function);





