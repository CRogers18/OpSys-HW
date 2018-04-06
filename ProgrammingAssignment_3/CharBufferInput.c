#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Input Driver, totally not a virus bro");
MODULE_AUTHOR("Group 66");

static int timesOpened = 0;
int currentPos = 0;
char mainBuffer[1024] = "";
EXPORT_SYMBOL(currentPos);
EXPORT_SYMBOL(mainBuffer);
 
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);
static DEFINE_MUTEX(buffer_mutex);
EXPORT_SYMBOL(buffer_mutex);
 
static struct file_operations fileOps =
{
    .open = dev_open,
    .write = dev_write,
    .release = dev_release,
};
 
int init_module(void)
{	
    int num = register_chrdev(42,"CharBufferInput", &fileOps);
 
    if(num < 0)
        printk(KERN_INFO "[ERROR] Input Device failed to register :(\n");
    else
        printk(KERN_INFO "[INIT] Input Device registered successfully\n");

    mutex_init(&buffer_mutex);
 
    return num;
}
 
void cleanup_module(void)
{
    unregister_chrdev(42,"CharBufferInput");
 
    printk(KERN_INFO "[CLEANUP] Input Device un-registered successfully\n");

    mutex_destroy(&buffer_mutex);
}
 
static int dev_open(struct inode *inod, struct file *fil)
{
    if(!mutex_trylock(&buffer_mutex))
    {
            printk(KERN_ALERT "Input Device is in use by another process!");
            return 0;
    }

    printk(KERN_ALERT "Input Device has been opened %d times\n", ++timesOpened);
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
	mutex_unlock(&buffer_mutex);
	
    printk(KERN_INFO "[RELEASE] CharBufferInput Device was closed... maybe...\n");
	
	
    return 0;
}
