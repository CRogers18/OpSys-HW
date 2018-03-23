#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Driver, totally not a virus");
MODULE_AUTHOR("Group 66");
 
static char mainBuffer[100];
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
        printk(KERN_INFO "Device failed to register :(\n");
    else
        printk(KERN_INFO "Device registered successfully\n");
 
    return num;
}
 
void cleanup_module(void)
{
    unregister_chrdev(42,"NSACharBuffer");
 
    printk(KERN_INFO "Device un-registered successfully\n");
}
 
static int dev_open(struct inode *inod, struct file *fil)
{
    printk(KERN_ALERT "Device has been opened %d times\n", ++timesOpened);
    return 0;
}
 
static ssize_t dev_read(struct file *filp, char* buff, size_t len, loff_t* off)
{
   unsigned short i = 0, bytesRead = 0;

   printk(KERN_INFO "Reading from device.....\n");
   // Needs to handle if len is greater than or less than current position
   while(i < currentPos)
    {
        // WARNING: Possible overflow on buff++ here
        printk(KERN_INFO "Read %c from the buffer.\n", mainBuffer[i]);
        put_user(mainBuffer[i], buff++);
        bytesRead++;
        i++;
    }

   if(len > currentPos){
      printk(KERN_ALERT "Not enough data to be read. Only %d bytes available to be read.\n", currentPos);
      currentPos = 0;
      return len;
   }
    
   currentPos = 0;
 
   return bytesRead;
}
 
static ssize_t dev_write(struct file *filp, const char* buff, size_t len, loff_t* off)
{
    unsigned short bytesWritten = 0, i = 0;
   
    printk(KERN_INFO "Writting to the device...\n");

   for(i = 0; i < len-1; i++)
    {
        if(currentPos == 1000)
        {
            printk(KERN_INFO "Buffer full, %d bytes were written.\n", bytesWritten);
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
    printk(KERN_INFO "NSACharBuffer Device was closed... maybe...\n");
    return 0;
}
