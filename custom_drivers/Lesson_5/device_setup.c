/**
 *  We are creating this platform device file for 2 devices.
 */ 

#include <linux/module.h>
#include <linux/platform_device.h> /* all platform related functions & structs in this file */

#include "platform.h" /* Created for this project to keep platform device info related struct */

/* 1. Create 2 platform data 
 *  Also this platform data should be part of your platform device struct because that is 
 *  how it will be supplied to the platform driver.
 *
 *  So now if you look at platform_device structure there is no field to add this data, 
 *  but platform_device struct has another struct inside it : struct device, inside it
 *  you will find a field void* platform_data, thats why in step 2 in structure you will
 *  see a field called .dev 
 * 
 *
 * */
struct pcdev_platform_data pcdev_pdata[2] = {

	[0] = {.size = 512, .perm = RDWR, .serial_number = "PCDEVABC1111"},
	[1] = {.size = 1024,.perm = RDWR, .serial_number = "PCDEVXYZ2222"}
};

/* 2. Create 2 platform devices */
struct platform_device platform_pcdev_1 = {
        .name = "pseudo-char-device",
        .id   = 0,
	.dev  = {
		.platform_data = &pcdev_pdata[0]
	}	
};

struct platform_device platform_pcdev_2 = {
        .name = "pseudo-char-device",
        .id   = 1,
	.dev = {
		.platform_data = &pcdev_pdata[1]
	}
};



static int __init pcdev_platform_init(void)
{
	// register platform device
	platform_device_register(&platform_pcdev_1);
	platform_device_register(&platform_pcdev_2);

	return 0;
}


static void __exit pcdev_platform_exit(void)
{
	platform_device_unregister(&platform_pcdev_1);
	platform_device_unregister(&platform_pcdev_2);
}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which register platform devices");

