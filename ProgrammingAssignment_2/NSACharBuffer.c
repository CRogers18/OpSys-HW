#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Driver, totally not a virus bro");
MODULE_AUTHOR("Group 66");

static char mainBuffer[1024];
static int currentPos = 0, timesOpened = 0;
 
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);
 
static struct file_operations fileOps =
{
    .read = dev_read,
    .open = dev_open,
    .write = dev_write,
    .release = dev_release,
};
 
int init_module(void)
{
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
 
static ssize_t dev_read(struct file *filp, char* buff, size_t len, loff_t* off)
{
    unsigned short i = 0, buffSize = 0;
   
    if(currentPos == 0)
    {
        printk(KERN_INFO "[ERROR] Tried to read from an empty buffer\n");
        return 0;
    }
 
    for(i = 0; i < len; i++)
    {
        if(i == currentPos)
        {
            printk(KERN_INFO "[ERROR] No data beyond %d bytes, cannot fully service request of %d bytes", currentPos, len);
            currentPos = 0;
            return buffSize;
        }
 
        printk(KERN_INFO "Read %c from the buffer.\n", mainBuffer[i]);
        put_user(mainBuffer[i], buff++);
        buffSize++;
    }
 
    for(i = len; i < currentPos; i++)
        mainBuffer[i-len] = mainBuffer[i];
       
    currentPos -= len;
 
    return buffSize;
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
    printk(KERN_INFO "[RELEASE] NSACharBuffer Device was closed... maybe...\n");
    return 0;
}
