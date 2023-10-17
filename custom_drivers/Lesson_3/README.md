## FILE OPERATIONS ##

In this lesson we are going to learn about different file operations: read, write, lseek, open and implement them in main.c.


In struct file there is variable loff_t f_pos which points initially to zero when a file is open. Because several processes may acccess the same file concurrently, the file pointer must be kept in 
the file object rather than the inode object.

struct file also contains variable: const struct file_operations * f_op; // when process open the file , the VFS initializes the f_op field of the new file object with the address stored in the inode 
so that further calls to file operations can use these functions.

NOTE: list of errors is given in include/uapi/asm-generic/errno-base.h eg error: EFAULT, ENOMEM, EFBIG. you should always return relevant error.

Now please checkout the implementation of methods in main.c. Once you go through it. now we can test it, so try:

1. sudo -s
2. echo "hello world" > /dev/pcd
3. cat /dev/pcd
4. you can also cp testFile /dev/pcd to copy an entire file.


Another thing we are doing in this exercise is to add error handling. 
