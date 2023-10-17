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

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void pcdev_release(struct device *dev)
{
	pr_info("Device released \n");
}


/*1. create n platform data */
struct pcdev_platform_data  pcdev_pdata[] = {
	[0] = {.size = 512, .perm = RDWR,   .serial_number = "PCDEVABC1111"},
	[1] = {.size = 1024,.perm = RDWR,   .serial_number = "PCDEVXYZ2222"},
	[2] = {.size = 128, .perm = RDONLY, .serial_number = "PCDEVXYZ3333"},
	[3] = {.size = 32,  .perm = WRONLY, .serial_number = "PCDEVXYZ4444"}
};

/*2. create n platform devices */ 

struct platform_device platform_pcdev_1 = {
	.name = "pcdev-A1x",
	.id = 0,
	.dev = {
		.platform_data = &pcdev_pdata[0],
		.release = pcdev_release
	}
};


struct platform_device platform_pcdev_2 = {
	.name = "pcdev-B1x",
	.id = 1,
	.dev = {
		.platform_data = &pcdev_pdata[1],
		.release = pcdev_release
	}
};


struct platform_device platform_pcdev_3 = {
	.name = "pcdev-C1x",
	.id = 2,
	.dev = {
		.platform_data = &pcdev_pdata[2],
		.release = pcdev_release
	}
};


struct platform_device platform_pcdev_4 = {
	.name = "pcdev-D1x",
	.id = 3,
	.dev = {
		.platform_data = &pcdev_pdata[3],
		.release = pcdev_release
	}
};


struct platform_device *platform_pcdevs[] = 
{
	&platform_pcdev_1,
	&platform_pcdev_2,
	&platform_pcdev_3,
	&platform_pcdev_4
};

static int __init pcdev_platform_init(void)
{
	/* register n platform devices */

	//platform_device_register(&platform_pcdev_1);
	//platform_device_register(&platform_pcdev_2);
	
	platform_add_devices(platform_pcdevs,ARRAY_SIZE(platform_pcdevs) );

	pr_info("Device setup module loaded \n");

	return 0;
}


static void __exit pcdev_platform_exit(void)
{

	platform_device_unregister(&platform_pcdev_1);
	platform_device_unregister(&platform_pcdev_2);
	platform_device_unregister(&platform_pcdev_3);
	platform_device_unregister(&platform_pcdev_4);
	pr_info("Device setup module unloaded \n");


}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which registers n platform devices ");
MODULE_AUTHOR("Arjun Kalsi");
