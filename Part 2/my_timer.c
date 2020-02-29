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

static struct file_operations operations; // Points to proc file definitions.
static char *string1, *string2;
static int readProc;
static int procReadNumber = 0;
struct timespec currentTime;
long elapsedSecs;
long elapsed_ns;

unsigned long copy_to_user(void __user *to,const void *from, unsigned long size);

static struct Time
{
    long seconds;
    long nanoseconds;
} time;

static int InitializeModule(void)
{
    printk(KERN_NOTICE "Creating /proc/%s.\n", MODULE_NAME);

    operations.open = OpenProc;
    operations.read = ReadProc;
    operations.release = ReleaseProc;

    if (!proc_create(MODULE_NAME, MODULE_PERMISSIONS, MODULE_PARENT, &operations))
    {
        printk(KERN_WARNING "Error: could not create proc entry.\n");
        remove_proc_entry(MODULE_NAME, MODULE_PARENT);

        return -ENOMEM;
    }

    return 0;
}

int OpenProc(struct inode *sp_inode, struct file *sp_file)
{
    printk(KERN_INFO "OpenProc() called.\n");
    readProc = 1;

    procReadNumber++;

    string1 = kmalloc(sizeof(char) * STRING_LENGTH, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
    string2 = kmalloc(sizeof(char) * STRING_LENGTH, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
    if (string1 == NULL || string2 == NULL) {
        printk(KERN_WARNING "Error: could not allocate memory for a string.");

        return -ENOMEM;
    }

    currentTime = current_kernel_time();
    sprintf(string1, "Current time: %ld.%ld\n", currentTime.tv_sec, currentTime.tv_nsec);

    if(currentTime.tv_nsec - time.nanoseconds < 0)
    {
        elapsedSecs = currentTime.tv_sec - time.seconds - 1;
        elapsed_ns = currentTime.tv_nsec - time.nanoseconds + 1000000000;
    }
    else
    {
        elapsedSecs = currentTime.tv_sec - time.seconds;
        elapsed_ns = currentTime.tv_nsec - time.nanoseconds;
    }

    if (procReadNumber > 1)
    {
        sprintf(string2, "Elapsed time: %ld.%ld\n", elapsedSecs, elapsed_ns);
        strcat(string1, string2);
    }

    time.seconds = currentTime.tv_sec;
    time.nanoseconds = currentTime.tv_nsec;

    return 0;
}

ssize_t ReadProc(struct file *sp_file, char __user *buffer, size_t size, loff_t *offset)
{
    int length = strlen(string1);

    readProc = !readProc;
    if (readProc == 1)
    {
        return 0;
    }

    printk(KERN_INFO "ReadProc() called.\n");
    copy_to_user(buffer, string1, length);

    return length;
}

int ReleaseProc(struct inode *sp_inode, struct file *sp_file)
{
    printk(KERN_NOTICE "ReleaseProc() has been called.\n");
    kfree(string1);
    kfree(string2);

    return 0;
}



static void ExitModule(void)
{
    printk(KERN_NOTICE "Removing /proc/%s.\n", MODULE_NAME);
    remove_proc_entry(MODULE_NAME, MODULE_PARENT);
}

module_init(InitializeModule);
module_exit(ExitModule);
