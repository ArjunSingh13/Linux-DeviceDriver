##PLATFORM DRIVERS##

In this lesson we will implement platform drivers.

So platform bus is a pseudo bus or virtual bus. It does not have any physical existence. it is linux terminology that do not have auto discoverable & hot plugging capability.
Because in SoC it is vendor specific, they have their own design and certain set of protocols that the implement unless other drivers such as wifi or usb.

In SoC there is no central bus system such as PCIe to find all on/off chip peripherals. So to all these non discoverable buses linux call it platform bus. so on this bus
we connect most of our embedded sensors/modules.


###Discovery of Devices###

- Every device has its configuration data and resources, which need to be reported to the OS.
- With this data OS learn about connected devices automatically (Device enumeration).
- Enumeration is a process through which OS can inquire and receive info such as type of device, manufacturer, device config etc of all connected devices.
- Once info is gathered by OS it can autoload appropriate driver for device. Eg buses USB, PCI support enumeration/hot plugging.
- However, for embedded system platforms, this may not be the case since most peripherals are connected to CPU over buses that dont support auto discovery. 
- That is why for embedded platform devices information must be fed to linux kernel manually either at compile time or at boot time of the kernel.


###Device Information###

It can be: 
memory or I/O mapped base address and range information, IRQ number on which the device issues interrupt to the processor, device identification number, DMA channel info,
device address, pin configuration, power/voltage param etc.


1. During compilation of kernel ( static and deprecated method)
2. Loading dynamically ( as a kernel module and not recommended)
3. During kernel boot ( Device tree blob, latest, recommended)

In board file approch, you need to add hardware configuration details to the kernel and need to recompile the kernel if any device property changes.

In device tree method, OS uses the device tree to discover the topology of the hardware at runtime and thereby support a majority of available hardware without hard coded information. 

In the old linux kernel version they use board file method and in arch/arm/mach-omap2 you can find omap3beagle boardfile for BBB which has everything hardcoded for BBB. in newer kernel version all these 
files are removed and there is just generic board file and all board peripheral detail is taken from device tree file.

TI gives drivers for general peripherals: SPI, I2C, USB, CAN etc. Eg location drivers/spi/spi-omap2-mcspi.c. These are called controller drivers which means these drivers are for on chip peripheral such
as I2C now this I2C peripheral can be for some sensor such as temperature sensor(I2C client). So important to note when someone say they writing i2c driver, it means it is generally for one or the other
sensor or module they connect on I2C bus and not for the I2C controller of SoC itself.


###Registering a platform driver ###

We are going to cover deprecated method first as understanding it explains a lot of details about how platform drivers work.

#### Here we have an important question ##### 
How do you make the correct driver gets autoloaded whenever you add the new platform device? answer is due to "matching" mechanism of the bus core. This matching is taken care by linux kernel itself
as long as you are following certain rules correctly. Linux Platform core implementation maintains a list of devices and drivers. whenever you add a new platform device or driver, this list gets updated
and matching mechanism triggers.

Now when a new device is added to list, its name gets compared by linux kernel to names in drivers list. When a match is detected probe() function of driver gets called with the name of device. 

Same way lets say you created and added a new driver, now it gets added in driver list of platform bus. Now matching mechanism of kernel gets trigger and lets say it found a device with matching name.
Now probe method of driver will get called for matching device and all information of matching device is passed onto the driver, so it can extract the platform data and configure it.

#### Probe function ####
- It must be implemented in driver and should be registered during platform_driver_register().
- when bus matching function detect the matching device and driver, probe function of the driver gets called with detected platform device as input arg.
- probe() should in general verify that the specified device hardware actually exists. 
- probe() is responsible for : Device detection and initialization, allocation of memory for various data structures, mapping i/o memory, registering interrupt handlers, registering device to kernel
framework.
- probe may return 0 (Success) or error code. 


#### Remove function ####
- called when platform device is removed from the kernel to unbind a device from the driver or when kernel no longer users the platform device.
- remove is responsible for : unregistering device from kernel framework, free memory if allocated on behalf of device, shutdown the device.

#### Other important functions ####
- shutdown : called at shutdown time to quiesce the device
- suspend : called to put the device to sleep mode
- resume : called to bring a device from sleep mode


### Exercise to implement platform device driver ###

Pseudo character driver we will implement as Platform driver.

we will create two kernel implementation:
- platform driver
- platform device (for device setup)

####  Platform device setup ####
- Create 2 platform devices and initialize them with info:
name of platform device, platform data, id of the device, release function for device.

- Register platform device with kernel.







