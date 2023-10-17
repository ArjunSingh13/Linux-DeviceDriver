#include <linux/module.h>
#include <linux/fs.h> // alloc_chrdev_region()
#include <linux/cdev.h> // for cdev operations

#define DEV_MEM_SIZE 512 // Buffer size of character driver
			 
//#define pr_fmt(fmt) fmt


/* Pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* To hold device number */
dev_t device_number;

/* For registering Character device operations with VFS */
struct cdev pcd_cdev;

/* File operations of the driver */
struct file_operations pcd_fops;

/* File operations that we register */
loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{
	pr_info("lseek requested\n");
	return 0;
}

/*
 * Here __user means this buff in kernel space is coming from user space.
 */
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("read requested for %zu bytes\n", count);
	return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("write requested for %zu bytes\n", count);
	return 0;
}

/*
 *  When we try to open device file this method gets called.
 */
int pcd_open(struct inode *inode, struct file *filp)
{
	pr_info("Open was successful\n");
	return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
	pr_info("Close was successful\n");
	return 0;
}

/* file operations of the driver */
struct file_operations pcd_fops = 
{
	.open    = pcd_open,
	.write   = pcd_write,
	.read    = pcd_read,
	.llseek  = pcd_lseek,
	.release = pcd_release,
	.owner   = THIS_MODULE	
};

/* to create class */
struct class* class_pcd;

/* to receive device */
struct device* device_pcd;


static int __init pcd_driver_init(void)
{
	// 1. Dynamically allocate a device number 
	// Args are: &dev, baseminor, count, *name
	alloc_chrdev_region(&device_number, 0, 1, "PCD_devices");
	pr_info("%s : device number <major>:<minor>= %d:%d\n", __func__ ,  MAJOR(device_number), MINOR(device_number));

	// 2. Initialize cdev structure with file operations
	// Args are :   cdev, file operations
	cdev_init(&pcd_cdev, &pcd_fops);

	// 3. Register a device with (cdev struct) VFS
	// argument 3rd is total number of devices that needs to be registered
	pcd_cdev.owner = THIS_MODULE; // this owner is setup here and not before cdev_init , because init initialize owner to null.
	cdev_add(&pcd_cdev, device_number, 1);


	// 4. we need to register file operations for this driver, their prototypes are given in struct file_operations in include/linux/fs.h file
	// later when we do oper("/dev/pcd",...) on the device file it sends request to kernel space in VFS. VFS open the file by  creating new object 
	// and linking it to corresponding inode object. Now VFS finds the correct driver and then pass to arguments : inode and filp.
	//
	// beside struct cdev and struct file_ops there are other 2 important structures: struct inode and struct file
	//
	// Unix makes a clear distinction between the contents of a file and the information about file. VFS data structure (struct inode) holds general info about file
	//
	// VFS 'file' data structure (struct file) tracks interaction on an opened file by user process. whenever a file is opened a file object is created in kernel space.
	// It stores interaction between open file and process.
	//
	// inode contains all info needed by the filesystem to handle a file, each file has its own inode object, which the filesystem uses to identify the file.
	//
	// Each inode object has unique inode number to uniquely identify the file within file system. so whenever u create a file an inode object is created. 
	//
	// strut file information exists only in kernel space whereas for inode it gets stored in memory.
	// eventually when you do open at user layer on a device file, whatever file operations register for it, that comes into play.
	//
	// SUMMARY: 
	// 4.1. When device file gets created using udev: "inode object gets created" in memory and inode's i_rdev field is init with device number.
	//    inode object's i_fop field is set to dummy default file operations.
	//
	// 4.2. When user process executes open system call on device file: "file object gets created",  inode's fop is copied to file object's fop(dummy file operations)
	//    Now inode->cdev->fops get copied into file->f_op and finally real kernel defined open function get called: file->f_op->open
	//	(Note: because in step 2 you updated yourself your file operations that become part of cdev struct which is inside inode struct)
	//
	// Now for file operation methods you pass inode and file structure as arguments
	
	// In the include/linux folder there is fs.h that has file_operations function with required function pointers declaration. copy them here in file.
	// Define these file operation functions and register them to structure for file_operations
	
	
	// 5. so that we dont need to create our device manually under /dev/ , we call class and  and device create apis. What they do is create a dev entry in 
	//   /sys/class/your_class_name/your_device_name/dev
	//
	//   Now there is a daemon called udevd that checks for hot plugins or wait for uEvent to occur that occur when we call device_create. Now this udev deamon receive uEvent and it looks 
	//   at dev file at the path given above and using that it creates device node for your device under /dev/ directory
	
	// create device class under /sys/class
	class_pcd = class_create(THIS_MODULE, "pcd_class");

	// populate the sysfd with device information
	device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

	// with above two functions in /sys/class now we will have a folder name pcd_class and then inside it pcd as the device.
	// that pcd will have dev and uevent. you can cat them to see device number.
	pr_info("PCD Module init was successful\n");
		
	
	return 0;
}

/*
 *	Clean up code has to be chronologically opposite order than initialize function	
 *
 */
static void __exit pcd_driver_cleanup(void)
{
	device_destroy(class_pcd, device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number, 1);
	pr_info("module unloaded\n");
}


module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A psuedo character device");


