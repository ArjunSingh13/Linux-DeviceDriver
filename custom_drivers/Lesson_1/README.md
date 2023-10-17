## Guide on learning and trying lesson 1 on your own ##

When building the Linux kernel, the resulting image is a single file made by the linking of all object files that correspond to features enabled in the configuration.

1. Static Modules: Part of kernel already, part of final kernel image.

2. Dynamic Modules: Loadable modules, can be loaded on demand. Does not result in increase in kernel image.
Each module is stored as separate file on the filesystem, using loadable moduels requires access to a filesystem.

Module loading is supported if in kernel config we have:
CONFIG_MODULES=y

Module Unloading supported with:
CONFIG_MODULE_UNLOAD=y

Now look at helloWorld.c file, it explains basic helloWorld kernel module and after that come back here.

## Building a linux kernel module: ##

Two ways:

1. Out of the kernel source tree (out-of-tree building). This does not allow integration into the kernel configuration/compilation process and module needs to be built seperately. Note: it only allows loadable kernel modules and modules can't be statically linked in the final kernel image.

2. Inside the kernel tree: you can produce either statically or dynamically loadable kernel module.


### Building out of tree module for host: ###

1. Out of tree modules are built using "kbuild" : it is build system used in linux kernel.
2. To build external modules, you must have a prebuilt kernel source available that contains the configuration and header files used in the build.

Note: you cant build against one linux kernel version and load it into the system which is running kernel of different version. 

3. Command to build external module : make -C <path to linux kernel tree> M=<path to your module> [target]

Here top level linux kernel source tree get executed and then we have to direct top level makefile to our local makefile with variable M in above command.

Now "target" in above command: modules, modules_install, clean, help


In local makefile : obj-<X> := <module_name>.o

here X can be:

n : Do not compile the module
y : Compile the module and link with kernel image
m : Compile as dynamically loadable kernel module

Example: obj-m := main.o


Now I created a makefile at this point, you can go check it and come back here after. 


Important :

you can build your module for host or target machine. For building against host, you need pre build kernel source of your Linux kernel version which is running on your host system. uname -r command to see
linux version. Also you can find source at /lib/modules/($uname -r)

Sometimes linux has not updated uname -r to latest kernel yet and in that case just go to /lib/modules and see if there is a later version than uname -r here so just use that manually.

so the command become:
make -C /lib/modules/($uname -r)/build M=($PWD) modules // here modules because we want to build module and that is the target.


Once your module is built successfully , you can link it to kernel:

sudo insmod main.ko

Now use "dmesg" to see messages given by kernel after loading out of tree module.

Remove from kernel command: sudo rmmod main.ko


### Building out tree module for Target ###

Note: if there is any error build for target, run following commands:
1. sudo apt-get update
2. sudo apt-get install gcc-arm-linux-gnueabihf
3. cat /home/username/.bashrc and take toolchain path from here and paste into sercure_path of  /etc/sudoers file.
4. Now with the help of makefile do make all and it should build for BBB.
5. Useful commands: arm-linux-gnueabihf-objdump -h main.ko, modinfo main.ko and file main.ko (shows that its built for ARM)
6. Now copy file to BBB: scp main.ko debian@192.168.7.2:/home/debian/work/drivers/ 
7. Now you should have a copy , so just insmod it and check with dmesg.

## Building in tree module ##
1. Add linux kernel module inside the linux kernel source tree and let linux build-system builds that.
2. If you want to list your kernel module selection in kernel menuconfig, then create and use a Kconfig file.

### Steps to add in tree module to kernel menu configuration
1. Create a folder in drivers/char/my_c_dev
2. Copy main.c
3. Create Kconfig file and add:
	menu "my custom modules"
	  config CUSTOM_HELLOWORLD
		tristate "hello world module support"
		default m
	endmenu
4. Add the local Kconfig entry to upper level Kconfig
5. Create a local Makefile
6. Add obj-<config_item>+= <module>.o
7. Add the local level makefile to higher level makefile
8. at top most Makefile in linux kernel do make ARCH=arm menuconfig , with this you can check out your custom char driver and change its setting
9. Buils kernel by running command: make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j4

TODO: from here next step is to build uImage and DTB so we can transfer them to  target. If you want to build the Linux image you have to use
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage dtbs LOADADDR=0x80008000 -j4


## Common Issues ##

1. Getting any kind of build errors: In this case just double check if your code is correct and matches exactly to what i have written in main.c and also match Makefile.
2. Unable to install main.ko to host: possible uname -r is pointing to older linux kernel version. Please look for the latest one in your host machine and try to build your module against it.



































