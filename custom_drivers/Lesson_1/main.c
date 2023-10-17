#include <linux/module.h> // mandatory for all kernel modules
#include <linux/init.h>	  // needed for __init and __exit macros
			  
/**
 * This function is the entry point and corresponds to the function called
 * when the module is loaded (modprobe/insmod)
 *
 * Note: for static modules , init function gets run only once during boot
 * sequence and will never re run until reboot occur.
 */
static int __init helloworld_init(void) {

	pr_info("Hello world initialization!\n");  // wrapper to printk
	return 0;
}

/**
 * Clean up and exit point and corresponds to the function executed at
 * module unloading (rmmod or modprobe -r)
 *
 */
static void __exit helloworld_cleanup(void) {

	pr_info("Hello world exit\n");
}

module_init(helloworld_init); // identify corresponding function as entry point
module_exit(helloworld_cleanup); // identify corresponding function as exit point
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arjun Kalsi <arjunkalsi13@gmail.com>");
MODULE_DESCRIPTION("Linux Kernel Hello World driver");
MODULE_INFO(board, "Beaglebone black");

// you can dump the content of .modeinfo section of kernel module using the
// " objdump -d -j .modinfo " command on the given module. For cross-compiled module, use $(CROSS_COMPILE)objdump instead.
