#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Buffer Driver, totally not a virus bro");
MODULE_AUTHOR("Group 66");

static char mainBuffer[100];
static char champs[38] = "Undefeated 2018 National Champions UCF";
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
    unsigned short bytesWritten = 0, i = 0, j = 0;

 	printk(KERN_INFO "Buffer Length: %d\n", len);
    for(i = 0; i < len; i++)
    {
        if(currentPos == 100)
        {
            printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
            return len;
        }
        else
        {   

	        if((i == 0) && buff[i] != 'U' ) 
	        { // If the first character in the input buffer string is C or F we need to do some checks to see if
	    	  // a UCF input was built.
	            if(buff[i] == 'C' && currentPos != 0){ //If we get in a C and its at the start of the input buffer
	       		//Check the main buffer for the U and the input buffer for F and make sure there's stuff already in the mainbuffer
		            if(((i+1) < len) && (i+1) != '\n'){
		       			if(mainBuffer[currentPos - 1] == 'U' && buff[i + 1] == 'F'){
		       				currentPos = currentPos - 1; //Move back a position
		       				i = i + 1; //Move up i past the F
		       				// Insert Champs
		       				printk(KERN_INFO "[DEBUG] A wild UCF is spotted. Cause of C\n");
		       				for(j=0;j<38;j++){
		                   		if(currentPos == 100) //Make sure we are going over the bounds
		              			{
		            				printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
		            				return len;
		        				}
		        				mainBuffer[currentPos] = champs[j];
		        				printk(KERN_INFO "[DEBUG] mainBuffer inserted %c just now bro.\n", mainBuffer[currentPos]);
		        				currentPos++;
		        				bytesWritten++;
		               		}
		           		}else{ // If the U and F arent there then just insert the C and move on
	               			mainBuffer[currentPos] = buff[i];
	            	   		currentPos++;
	           				bytesWritten++;
	           				//continue;
	           			} 
	           		}else{ // If we got a C and current position is not 0 but there's nothing left in the input buffer to check
	           	    	// insert the C into the buffer and move on.
	               		mainBuffer[currentPos] = buff[i];
	            	   	currentPos++;
	           			bytesWritten++;
	           			//continue;
	           		}     	
	       		}
	       		if(buff[i] == 'F' && currentPos >= 2){ //If we get in an F at the beginning of the input buffer
	       			// Check if the U and C are in the main buffer and that there are more than or equal to 2 chars in the main buffer 
	       			if(mainBuffer[currentPos - 1] == 'C' && mainBuffer[currentPos - 2] == 'U'){
	       				currentPos = currentPos - 2; //Go back two spots in the current position and insert the champs
	       				printk(KERN_INFO "[DEBUG] A wild UCF is spotted. Cause of F\n");
	       				for(j=0;j<38;j++){
	                   		if(currentPos == 100) //Make sure we are going over the bounds
	              			{
	            				printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
	            				return len;
	        				}
	        				mainBuffer[currentPos] = champs[j];
	        				printk(KERN_INFO "[DEBUG] mainBuffer inserted %c just now bro.\n", mainBuffer[currentPos]);
	        				currentPos++;
	        				bytesWritten++;
	               		}	
	       			}
	       			else{ //I no C and U insert and move on
	               		mainBuffer[currentPos] = buff[i];
	            	   	currentPos++;
	           			bytesWritten++;
	           			//continue;
	       			}
	       		}


	       	}
            // Checking if UCF is read in at any position 
	        else if(buff[i] == 'U' && ((i+2) < len)) // Check for U and the next 2 characters in front of it safely
	       	{	
	   			// If the next 2 characters are 'CF'
	        	if(buff[i+1] == 'C' && buff[i+2] == 'F')
	          	{	// UCF is read in. Must Compensate for it
		           	printk(KERN_INFO "UCF was inserted into the buffer. Deal with it.\n");
		       		i = i + 2; // Move up i to get the rest of buff
		          	for(j=0;j<38;j++){
		                if(currentPos == 100) //Make sure we are going over the bounds
		               	{
		           			printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
		           			return len;
		       			}
		        			mainBuffer[currentPos] = champs[j];
		        			printk(KERN_INFO "[DEBUG] mainBuffer inserted %c just now bro.\n", mainBuffer[currentPos]);
		        			currentPos++;
		        			bytesWritten++;
		           		}
	       		}
	           	else{ // If not insert and move on
	               		mainBuffer[currentPos] = buff[i];
	            		currentPos++;
	            		bytesWritten++;
	               	}
	   		}
	
			else{

			printk(KERN_INFO "Inserted %c into the buffer.\n", buff[i]);
	 		mainBuffer[currentPos] = buff[i];
	 		currentPos++;
	        bytesWritten++;
	    }
       		
       	} // End else		
    } // End for loop

    
    return bytesWritten;
}
 
static int dev_release(struct inode* inod, struct file* fil)
{
    printk(KERN_INFO "[RELEASE] NSACharBuffer Device was closed... maybe...\n");
    return 0;
}