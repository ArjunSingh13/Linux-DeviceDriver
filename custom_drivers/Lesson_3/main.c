#include <linux/module.h>
#include <linux/fs.h> // alloc_chrdev_region()
#include <linux/cdev.h> // for cdev operations
#include <linux/uaccess.h> // copy from user , copy to user


#define DEV_MEM_SIZE 512 // Buffer size of character driver

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__


/* Pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* To hold device number */
dev_t device_number;

/* For registering Character device operations with VFS */
struct cdev pcd_cdev;

/* File operations of the driver */
struct file_operations pcd_fops;

/* File operations that we register */
loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
	loff_t temp;
	pr_info("lseek requested\n");
	pr_info("Current file position : %lld\n", filp->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0)) { return -EINVAL;}
			filp->f_pos = offset;
			break;

		/* adding to current cursor position */
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) ||  (temp < 0)) { return -EINVAL;}

			filp->f_pos += offset;
			break;

		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0)) { return -EINVAL;}  
			filp->f_pos = DEV_MEM_SIZE + offset;
			break;

		default:
			return -EINVAL;
	}

	pr_info("New file position : %lld\n", filp->f_pos);	
	return filp->f_pos;
}

/*
 * Here __user means this buff in kernel space is coming from user space.
 * VFS provide these function params for these kernel functions.
 */
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{

	pr_info("read requested for %zu bytes\n", count);
	pr_info("current file position : %lld\n", *f_pos);

	/* adjust the count */
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;	
	}


	/* copy to user */
	if(copy_to_user(buff, &device_buffer[*f_pos], count)) {
		return -EFAULT;
	}

	
	/*  update the current file position */
	*f_pos += count;
	
	pr_info("Number of bytes successfully read = %zu\n", count);
	
	/* as loff_t is typedef of long long int */
	pr_info("updated file position = %lld\n", *f_pos);


	/* Return number of bytes which have been successfully read */
	return count;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("write requested for %zu bytes\n", count);
        pr_info("current file position : %lld\n", *f_pos);

        /* adjust the count */
        if((*f_pos + count) > DEV_MEM_SIZE)
        {
                count = DEV_MEM_SIZE - *f_pos;
        }

	if(!count){ return -ENOMEM;}


        /* copy from user */
        if(copy_from_user(&device_buffer[*f_pos], buff, count)) {
                return -EFAULT;
        }


        /*  update the current file position */
        *f_pos += count;

        pr_info("Number of bytes successfully written = %zu\n", count);

        /* as loff_t is typedef of long long int */
        pr_info("updated file position = %ld\n", *f_pos);

	return count;
}
int pcd_open(struct inode *inode, struct file *filp)
{
	pr_info("Open was successful\n");
	return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
	pr_info("Release was successful\n");
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
	int ret=0;
	
	// 1. Dynamically allocate a device number 
	// Args are: &dev, baseminor, count, *name
	alloc_chrdev_region(&device_number, 0, 1, "PCD_devices");
	if(ret < 0){
		pr_err("Alloc chrdev failed\n");
		goto out;
	}
	pr_info("%s : device number <major>:<minor>= %d:%d\n", __func__ ,  MAJOR(device_number), MINOR(device_number));

	// 2. Initialize cdev structure with file operations
	// Args are :   cdev, file operations
	cdev_init(&pcd_cdev, &pcd_fops);

	// 3. Register a device with (cdev struct) VFS
	// argument 3rd is total number of devices that needs to be registered
	pcd_cdev.owner = THIS_MODULE; // this owner is setup here and not before cdev_init , because init initialize owner to null.
	ret = cdev_add(&pcd_cdev, device_number, 1);
	if(ret < 0){
		pr_err("Cdev add failed\n");
		goto unreg_chrdev;
	}
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
	
	/* In class_create() retVal is int that is converted to pointer type using ERR_PTR(retVal) macro, so rather than pointer to error code is returned
	 * Here macro IS_ERR is used to check if there is error */
	if(IS_ERR(class_pcd)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(class_pcd);
		goto cdev_del;
	}

	// populate the sysfd with device information
	device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
	if(IS_ERR(device_pcd)) {
			pr_err("Device create failed\n");
			ret = PTR_ERR(device_pcd);
			goto class_del;
	}

	// with above two functions in /sys/class now we will have a folder name pcd_class and then inside it pcd as the device.
	// that pcd will have dev and uevent. you can cat them to see device number.
	pr_info("PCD Module init was successful\n");
	return 0;

class_del:
	class_destroy(class_pcd);
cdev_del:
	cdev_del(&pcd_cdev);
unreg_chrdev:
	unregister_chrdev_region(device_number, 1);
out:
	pr_err("Module insertion failed\n");
	return ret;
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


