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
    unsigned short bytesWritten = 0, i = 0, j = 0, k = 0, letterCmpCount = 0, subStringExists = 0, insertPos;
    char currentChar;
    char *theChamps = "Undefeated 2018 National Champions UCF";
    char tempHold[35];

    // contains the indexes at which the substring can be found in the buffer, 1024 / 3 = about 341 positions in the buffer
    // so this has to be large enough to handle "UCFUCFUCF" etc. cases
    short positions[341] = {-1};

    // Determines if the input contains the substring 'UCF'
    for(i = 0; i < len; i++)
    {
        if(buff[i] == 'U')
        {
            // Check the next 2 characters in front of it safely
            if( (i+2) < len)
            {
                // If the next 2 characters are 'CF'
                if(buff[i+1] == 'C' && buff[i+2] == 'F')
                {
                    // Mark the position and continue
                    positions[k++] = i;
                    subStringExists = 1;
                }
            }
        }
    }

    // If the substring exists, we will need more room to write to the buffer
    if(subStringExists)
    {

        for(i = 0; i < 341; i++)
            if(positions[i] != -1)
                letterCmpCount++;
            else
                break;

        // This will likely extend beyond 1024 in some cases but the loop will halt it
        len += (letterCmpCount * 38);
    }

    for(i = 0; i < len; i++)
    {
        if(currentPos == 1024)
        {
            printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
            return len;
        }
        
        // Inserting into the buffer
        else
        {
            // If we're at an index where a replacement should happen
            if(subStringExists && (currentPos == positions[j])) 
            {
                // insert the replacement string
                for(k = 0; k < 38; k++)
                {
                    // halt if the buffer is about to overflow
                    if(currentPos == 1024)
                    {
                        printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
                        return len;
                    }

                    else
                    {
                        insertPos = positions[j];

                        // Temporarily hold any buffer data beyond 'UCF' if it exists
                        if(k > 2 && (insertPos < currentPos))
                            tempHold[k-3] = mainBuffer[insertPos+k];

                        printk(KERN_INFO "Inserted %c into the buffer.\n", theChamps[k]);
                        mainBuffer[currentPos] = theChamps[k];
                        currentPos++;
                        bytesWritten++;                        
                    }
                }

                // TODO: append old buffer data that was overwritten here

                // increment positions array index to next marked position
                j++;
            }

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
