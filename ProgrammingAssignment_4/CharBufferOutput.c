#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Output Driver, totally not a virus bro");
MODULE_AUTHOR("Group 66");

static int timesOpened = 0;
extern int currentPos;
extern char mainBuffer[1024]; 
 
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
extern struct mutex buffer_mutex; 

static struct file_operations fileOps =
{
    .read = dev_read,
    .open = dev_open,
    .release = dev_release,
};
 
int init_module(void)
{
    int num = register_chrdev(43,"CharBufferOutput", &fileOps);
 
    if(num < 0)
        printk(KERN_INFO "[ERROR] Output Device failed to register :(\n");
    else
        printk(KERN_INFO "[INIT] Output Device registered successfully\n");

    mutex_init(&buffer_mutex);

    return num;
}
 
void cleanup_module(void)
{
    unregister_chrdev(43,"CharBufferOutput");
 
    printk(KERN_INFO "[CLEANUP] Output Device un-registered successfully\n");

    mutex_destroy(&buffer_mutex);
}
 
static int dev_open(struct inode *inod, struct file *fil)
{
    if(!mutex_trylock(&buffer_mutex))
    {
        printk(KERN_ALERT "Output Device is in use by another process!");
        return 0;
    }

    printk(KERN_ALERT "Output Device has been opened %d times\n", ++timesOpened);
    return 0;
}
 
static ssize_t dev_read(struct file *filp, char* buff, size_t len, loff_t* off)
{
    unsigned short i = 0, buffSize = 0;
   
    if(currentPos == 0)
    {
        printk(KERN_INFO "[ERROR] Output Tried to read from an empty buffer\n");
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
 
static int dev_release(struct inode* inod, struct file* fil)
{
    mutex_unlock(&buffer_mutex);

    printk(KERN_INFO "[RELEASE] CharBufferOutput Device was closed... maybe...\n");
    return 0;
}
