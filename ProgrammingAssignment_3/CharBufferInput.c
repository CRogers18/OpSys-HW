#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Driver, totally not a virus bro");
MODULE_AUTHOR("Group 66");

//extern variables need to be initialized in order to compile
extern static char mainBuffer[1024] = {''};
static int currentPos = 0, timesOpened = 0;
 
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);
static DEFINE_MUTEX(input_mutex);
 
static struct file_operations fileOps =
{
    .read = dev_read,
    .open = dev_open,
    .write = dev_write,
    .release = dev_release,
};
 
int init_module(void)
{
	mutex_init(&input_mutex);
	
    int num = register_chrdev(42,"NSACharBuffer", &fileOps);
 
    if(num < 0)
        printk(KERN_INFO "[ERROR] Device failed to register :(\n");
    else
        printk(KERN_INFO "[INIT] Device registered successfully\n");
 
    return num;
}
 
void cleanup_module(void)
{
    unregister_chrdev(42,"NSACharBuffer");
 
    printk(KERN_INFO "[CLEANUP] Device un-registered successfully\n");
}
 
static int dev_open(struct inode *inod, struct file *fil)
{
    printk(KERN_ALERT "Device has been opened %d times\n", ++timesOpened);
    return 0;
}
 
static ssize_t dev_write(struct file *filp, const char* buff, size_t len, loff_t* off)
{
    unsigned short bytesWritten = 0, i = 0;
 
    for(i = 0; i < len; i++)
    {
        if(currentPos == 1024)
        {
            printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
            return len;
        }
		
		if(!mutex_trylock(&input_mutex))
		{
			printk(KERN_ALERT "Device is in use by another process!");
			return len;
		}
 
        else
        {
            printk(KERN_INFO "Inserted %c into the buffer.\n", buff[i]);
            mainBuffer[currentPos] = buff[i];
            currentPos++;
            bytesWritten++;
        }
    }
 
    return bytesWritten;
}
 
static int dev_release(struct inode* inod, struct file* fil)
{
	mutex_unlock(&input_mutex);
	
    printk(KERN_INFO "[RELEASE] NSACharBuffer Device was closed... maybe...\n");
	
	mutex_destroy(&input_mutex);
	
    return 0;
}
