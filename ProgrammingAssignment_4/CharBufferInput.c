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
static char champs[38] = "Undefeated 2018 National Champions UCF";
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
	unsigned short bytesWritten = 0, i = 0, j = 0;

    for(i = 0; i < len; i++)
    {
        if(currentPos == 1024)
        {
            printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
            return len;
        }
        else
        {   
	        if((i == 0) && buff[i] != 'U') 
	        { // If the first character in the input buffer string is C or F we need to do some checks to see if
	    	  // a UCF input was built.
	            if(buff[i] == 'C' && currentPos != 0)
				{ //If we get in a C and its at the start of the input buffer
	       		//Check the main buffer for the U and the input buffer for F and make sure there's stuff already in the mainbuffer
		            if(((i+1) < len) && (i+1) != '\n')
					{ //If the next thing in the buffer exists and isnt the newline character cont
		            	//If it is the newline character next move on
		       			if(mainBuffer[currentPos - 1] == 'U' && buff[i + 1] == 'F')
						{
		       				currentPos = currentPos - 1; //Move back a position
		       				i = i + 1; //Move up i past the F
		       				// Insert Champs
		       				//printk(KERN_INFO "[DEBUG] A wild UCF is spotted. Inserting Champs.\n");
		       				for(j=0;j<38;j++)
							{
		                   		if(currentPos == 1024) //Make sure we are going over the bounds
		              			{
		            				printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
		            				return len;
		        				}
		        				mainBuffer[currentPos] = champs[j];
		        				currentPos++;
		        				bytesWritten++;
		               		}
		           		}
						else
						{ // If the U and F arent there then just insert the C and move on
	               			mainBuffer[currentPos] = buff[i];
		        			//printk(KERN_INFO "[DEBUG] mainBuffer inserted %c\n", mainBuffer[currentPos]);
	            	   		currentPos++;
	           				bytesWritten++;
	           			} 
	           		}
					else
					{ // If we got a C and current position is not 0 but there's nothing left in the input buffer to check
	           	    	// insert the C into the buffer and move on.
	               		mainBuffer[currentPos] = buff[i];
		        		//printk(KERN_INFO "[DEBUG] mainBuffer inserted %c\n", mainBuffer[currentPos]);	               		
	            	   	currentPos++;
	           			bytesWritten++;
	           		}     	
	       		}
	       		else if(buff[i] == 'F' && currentPos >= 2)
				{ //If we get in an F at the beginning of the input buffer
	       			// Check if the U and C are in the main buffer and that there are more than or equal to 2 chars in the main buffer 
	       			if(mainBuffer[currentPos - 1] == 'C' && mainBuffer[currentPos - 2] == 'U')
					{
	       				currentPos = currentPos - 2; //Go back two spots in the current position and insert the champs
		       			//printk(KERN_INFO "[DEBUG] A wild UCF is spotted. Inserting Champs.\n");
	       				for(j=0;j<38;j++)
						{
	                   		if(currentPos == 1024) //Make sure we are going over the bounds
	              			{
	            				printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
	            				return len;
	        				}
	        				mainBuffer[currentPos] = champs[j];
	        				currentPos++;
	        				bytesWritten++;
	               		}	
	       			}
	       			else
					{ //I no C and U insert and move on
	               		mainBuffer[currentPos] = buff[i];
	               		//printk(KERN_INFO "[DEBUG] mainBuffer inserted %c\n", mainBuffer[currentPos]);
	            	   	currentPos++;
	           			bytesWritten++;
	           			//continue;
	       			}
	       		}

	       		else
				{ // If were at the frst index and its not U C or F, insert and move on
	       			mainBuffer[currentPos] = buff[i];
	               	//printk(KERN_INFO "[DEBUG] mainBuffer inserted %c\n", mainBuffer[currentPos]);
	               	currentPos++;
	           		bytesWritten++;	
	       		}
	       	}
            // Checking if UCF is read in at any other position 
	        else if(buff[i] == 'U' && ((i+2) < len)) // Check for U and the next 2 characters in front of it safely
	       	{	
	   			// If the next 2 characters are 'CF'
	        	if(buff[i+1] == 'C' && buff[i+2] == 'F')
	          	{	// UCF is read in. Must Compensate for it
		       		i = i + 2; // Move up i to get the rest of buff
		       		//printk(KERN_INFO "[DEBUG] A wild UCF is spotted. Inserting Champs.\n");
		          	for(j=0;j<38;j++)
					{
		                if(currentPos == 1024) //Make sure we are going over the bounds
		               	{
		           			printk(KERN_INFO "[ERROR] Buffer full, %d bytes were written.\n", bytesWritten);
		           			return len;
		       			}
						
						mainBuffer[currentPos] = champs[j];
		        		currentPos++;
		        		bytesWritten++;
		           	}
	       		}
	           	else
				{ // If not insert and move on
	               	mainBuffer[currentPos] = buff[i];
		        		//printk(KERN_INFO "[DEBUG] mainBuffer inserted %c\n", mainBuffer[currentPos]);
	            	currentPos++;
	            	bytesWritten++;
	            }
	   		}
			else
			{ // If i != 0 and buff[i] != U and every other case. Insert like normal

				printk(KERN_INFO "Inserting %c into the buffer.\n", buff[i]);
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
	mutex_unlock(&buffer_mutex);
	
    printk(KERN_INFO "[RELEASE] CharBufferInput Device was closed... maybe...\n");
	
	
    return 0;
}
