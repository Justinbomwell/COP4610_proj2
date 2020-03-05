#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>


MODULE_LICENSE("DUAL BSD/GPL");
MODULE_DESCRIPTION("Display the value of my_timer on each proc read.");

#define MODULE_NAME "timed"
#define MODULE_PERMISSIONS 0644
#define MODULE_PARENT NULL
#define STRING_LENGTH 256
#define MALLOC_FLAGS "__GFP_RECLAIM | __GFP_IO | __GFP_FS"

static struct file_operations ops; // Points to proc file definitions.
static char *string1, *string2;
static int readProc;
static int procReadNumber = 0;
struct timespec currentTime;
long elapsedSecs;
long elapsed_ns;

//Function declarations
static int timer_module_init(void);
int timer_proc_open(struct inode *sp_inode, struct file *structfile);
ssize_t timer_read_proc(struct file *file1, char __user*buffer, size_t size, loff_t *offset);
int timer_proc_release(struct inode *inode1, struct file *file2);
unsigned long copy_to_user(void __user *to,const void *from, unsigned long size);
static void timer_module_exit(void);

//time structure
static struct time
{
    long seconds;
    long nanoseconds;
} time;

//Initialize the module
static int timer_module_init(void)
{
    printk(KERN_NOTICE "Creating /proc/%s.\n", MODULE_NAME);

    ops.open = timer_proc_open;
    ops.read = timer_read_proc;
    ops.release = timer_proc_release;

    if (!proc_create(MODULE_NAME, MODULE_PERMISSIONS, MODULE_PARENT, &ops))
    {
        printk(KERN_WARNING "Error: could not create proc entry.\n");
        remove_proc_entry(MODULE_NAME, MODULE_PARENT);

        return -ENOMEM;
    }

    return 0;
}


//OPEN THE PROC
int timer_proc_open(struct inode *sp_inode, struct file *structfile)
{
    printk(KERN_INFO "timer_proc_open() called.\n");
    readProc = 1;

    procReadNumber++;		//Increase number for display purposes

    string1 = kmalloc(sizeof(char) * STRING_LENGTH, MALLOC_FLAGS);
    string2 = kmalloc(sizeof(char) * STRING_LENGTH, MALLOC_FLAGS);
    if (string1 == NULL || string2 == NULL) {
        printk(KERN_WARNING "Error: could not allocate memory.");

        return -ENOMEM;
    }

    currentTime = current_kernel_time();
    sprintf(string1, "current time: %ld.%ld\n", currentTime.tv_sec, currentTime.tv_nsec);

    if(currentTime.tv_nsec - time.nanoseconds < 0)		//Math to compensate for sec/ns values
    {
        elapsedSecs = currentTime.tv_sec - time.seconds - 1;
        elapsed_ns = currentTime.tv_nsec - time.nanoseconds + 1000000000;	//"adds one second's time"
    }
    else
    {
        elapsedSecs = currentTime.tv_sec - time.seconds;
        elapsed_ns = currentTime.tv_nsec - time.nanoseconds;
    }

    if (procReadNumber > 1)		//If it's not the first read, display elapsed time.
    {
        sprintf(string2, "elapsed time: %ld.%ld\n", elapsedSecs, elapsed_ns);
        strcat(string1, string2);
    }

    time.seconds = currentTime.tv_sec;
    time.nanoseconds = currentTime.tv_nsec;

    return 0;
}

ssize_t timer_read_proc(struct file *file1, char __user

*buffer, size_t size, loff_t *offset)
{
int length = strlen(string1);

readProc = !readProc;
if (readProc == 1)
{
return 0;
}

printk(KERN_INFO "timer_read_proc() called.\n");
copy_to_user(buffer, string1, length);

return length;
}

int timer_proc_release(struct inode *inode1, struct file *file2)
{
    printk(KERN_NOTICE "timer_release_proc() has been called.\n");
    kfree(string1);
    kfree(string2);

    return 0;
}


static void timer_module_exit(void)
{
    printk(KERN_NOTICE "Removing /proc/%s.\n", MODULE_NAME);
    remove_proc_entry(MODULE_NAME, MODULE_PARENT);
}

module_init(timer_module_init);
module_exit(timer_module_exit);
