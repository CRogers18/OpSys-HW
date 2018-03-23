#include <linux/init.h>           
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>         
#include <linux/fs.h>             
#include <linux/uaccess.h>         
#define  DEVICE_NAME "driver"    

MODULE_LICENSE("GPL");                    // The license type
MODULE_DESCRIPTION("Programming Assignment 2");  
MODULE_VERSION("1.0");            
MODULE_AUTHOR("Group 66");

static char   msg[1024] = {0};           // Memory for the string that is passed from userspace
static short  currentPos = 0;          // Used to remember the position on the Character array
static int    numberOpens = 0;           // Counts the number of times the device is opened

// The prototype functions for the character driver 
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

//Initialize the Module
int init_module(void){
   printk(KERN_INFO "Initializing Driver Module\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Failed to register...\n");
      return majorNumber;
   }

   printk(KERN_INFO "Device class created correctly\n"); // Made it! device was initialized
   return 0;
}

//Clean up the module
void cleanup_module(void){

   unregister_chrdev(majorNumber, DEVICE_NAME);    // unregister the major number
   printk(KERN_INFO "Goodbye from the LKM!\n");
}

// Open the device
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "This Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filp, char* buff, size_t len, loff_t* off)
{
    unsigned short i = 0, bytesRead = 0;
   
    printk(KERN_INFO "Device being read now...\n");

    while(i < currentPos)
    {
        // WARNING: Possible overflow on buff++ here
        put_user(mainBuffer[i], buff++);
        bytesRead++;
        i++;
    }
 
    currentPos = 0;
 
    return bytesRead;
}
 
static ssize_t dev_write(struct file *filp, const char* buff, size_t len, loff_t* off)
{
    unsigned short bytesWritten = 0, i = 0;
   
   printk(KERN_INFO "Device being written to now...\n");

    for(i = 0; i < len; i++)
    {
        if(currentPos == 1024)
        {
            printk(KERN_INFO "Buffer full, %d bytes were written.", bytesWritten);
            break;
        }
        else
        {
            mainBuffer[currentPos] = buff[i];
            currentPos++;
            bytesWritten++;
        }
    }
 
    return bytesWritten;
}


static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}

