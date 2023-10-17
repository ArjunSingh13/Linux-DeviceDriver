** Driver to handle multiple pseudo character devices **

Now driver needs to first determine which device is trying to access device first.


In this example there are going to be 4 devices:

/dev/pcdev-1 : RDONLY
/dev/pcdev-2 : WRONLY
/dev/pcdev-3 : RDWR
/dev/pvdev-4 : RDWR

Now Pseudo device driver needs to have 2 structures : 
1. To hold driver's private data.
2. To hold device's private data.

Please look in main.c to check both structures and information on them.

main.c has all the needed comments to understand the implementation so check pcd_driver_init first then open function and then rest of the functions.

*** TESTING ***

1. insmode main.ko.
2. check with dmesg and it should show insert successful.
3. you can check by ls -l /dev/pcdev- and press tab , it should show up 3 devices with major and minor number.
4. now you can use dd command to copy a file to a character driver, eg: dd if=main.c of=/dev/pcdev-2.
5. Now if you do dmesg it will show what happened in background.
6. dd try to write in blocks of 512 bytes ( it can be changed but its default).
7. adding count=1 at end of dd command will make it write only once. so it will not try again to write next 512 bytes of data.
8. Depending on if you have given read, write or read-write access those only operations permitted on the device.
9. "bs" command can be used to change block size.
10. use cat command to read what is written: cat /dev/pcdev-2.

Application to interact with your device (file name : app_dev_read.c) when you run this app with number of characters you want to read, it is currently giving error at open function.
it's a TODO and needs to be fixed.
 
