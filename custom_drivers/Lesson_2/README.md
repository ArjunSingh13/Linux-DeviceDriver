## Device Drivers Inroduction ##

you have user level programs including write, read, open that are pointed to device file example: /dev/xyz , now this device file interact with device driver of XYZ in Kernel space. and device driver takes care of interacting with Hardware.

Types:

1. Character driver ( Byte by byte data eg, keyboard, sensors etc)
2. Block driver    ( Chunk of data operated all together eg, sdmmc)
3. Network driver

example for character driver: user applicat n write(fd, 0xAB) -> goes to RTC driver -> finally to RTC device.


### Device Files ###

- Now device files are accessed as files in linux. It is special file in /dev that populates on kernel boot time or device hot plug events (run time)
- With help of device file user application and device can communicate.
- Device files are managed as part of VFS subsystem of the kernel.

- ls -l in /dev folder you can see various char, block , network drivers.


## Character Driver ##

- So there are functions open, read, write at user level so when you do open("/dev/rtc"...) it eventually call open of kernel level that interact with hardware device and respond back with result at user
level.
- There is VFS (virtual file system , here you can learn more about it : https://docs.kernel.org/filesystems/vfs.html#:~:text=The%20Virtual%20File%20System%20(also,different%20filesystem%20implementations%20to%20coexist.) that helps for user level calls to make communicate with kernel calls. 
- for this reason driver need to be registered to VFS, so when there is call VFS looks into driver registration list and points the request to correct driver.



#### Device Number ####

- for user application to point to right driver, there is a driver number associated with all drivers, and then a device number as well to make sure user application points to right device.
- eg, 4:0 to 4:4 where 4 is driver number and total 4 devices.
- ls -l in /dev folder also shows this device number, we also call it major minor number where major is driver and minor is for device.


#### Important steps ####

- Create device number (alloc_chrdev_region())
- Make a char device registration with the VFS (cdev_init() and cdev_add())
- Create device files (Class_create() and device_create())

NOTE: Device file is very important as its the interface between user application and acutal device. its in /dev/ folder as a normal file.

At this point you can go to main.c and look at the code to understand 

