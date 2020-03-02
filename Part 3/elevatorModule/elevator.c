#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/mutex.h>

MODULE_LICENSE("DUAL BSD/GPL");
MODULE_DESCRIPTION("Elevator scheduling for pet hotel.\n");

#define MODULE_NAME "elevator"
#define PERMS 0644
#define MODULE_PARENT NULL

#define OFFLINE 0
#define IDLE 1
#define LOADING 2
#define UP 3
#define DOWN 4

#define CAT_TYPE 1
#define DOG_TYPE 2

#define PERSON_WEIGHT 3
#define DOG_WEIGHT 2
#define CAT_WEIGHT 1

static struct file_operations fops;

struct list_head elevList;
struct list_head floorLists[10];

typedef struct person
{
	struct list_head list;
	int num_pets;
	int pet_type;
	int start_floor;
	int destination_floor;
	int direction;
	int weight;
} Person;

char *message;
int read_p;

extern long (*STUB_issue_request)(int, int, int, int);
long issue_request(int num_pets, int pet_type, int start_floor, int destination_floor)
{
	Person *p;

        printk(KERN_NOTICE "System call issue_request called.\n");

	if(num_pets < 0 || num_pets > 3) return 1;
	if(pet_type < 1 || pet_type > 2) return 1;
	if(start_floor < 1 || start_floor > 10) return 1;
	if(destination_floor < 1 || destination_floor > 10) return 1;

	p = kmalloc(sizeof(Person), __GFP_RECLAIM);
	p->num_pets = num_pets;
	p->pet_type = pet_type;
	p->start_floor = start_floor;
	p->destination_floor = destination_floor;

	if(start_floor > destination_floor) p->direction = DOWN;
	else if(start_floor < destination_floor) p->direction = UP;
	else if(start_floor == destination_floor)
	{
		//do something here
	}

	p->weight = 3 + num_pets*pet_type;

	list_add_tail(&p->list, &floorLists[start_floor]);

        return 0;
}

extern long (*STUB_start_elevator)(void);
long start_elevator(void)
{
        printk(KERN_NOTICE "System call start_elevator called.\n");



        return 0;
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void)
{
        printk(KERN_NOTICE "System call stop_elevator.\n");

        return 0;
}

int elevator_proc_open(struct inode *sp_inode, struct file *sp_file)
{
        int i;
	char * buf;

        printk(KERN_NOTICE "elevator_proc_open called.\n");
        read_p = 1;
        message = kmalloc(sizeof(char) * 2048, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	buf = kmalloc(sizeof(char) * 2048, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

        if (message == NULL)
        {
                printk(KERN_WARNING "ERROR: elevator_proc_open");
                return -ENOMEM;
        }

	if(buf == NULL)
	{
		printk(KERN_WARNING "ERROR: elevator_proc_open");
		return -ENOMEM;
	}

	strcpy(message, "");

        sprintf(buf, "Elevator State: %s\n", "OFFLINE");
	strcat(message, buf);
        sprintf(buf, "Elevator Animals: %s\n", "NONE");
	strcat(message, buf);
        sprintf(buf, "Current Floor: %d\n", 0);
	strcat(message, buf);
        sprintf(buf, "Number of Passengers: %d\n", 0);
	strcat(message, buf);
        sprintf(buf, "Current Weight: %d\n", 0);
	strcat(message, buf);
        sprintf(buf, "Number of Passengers Waiting: %d\n", 0);
	strcat(message, buf);
        sprintf(buf, "Number of Passengers Serviced %d\n", 0);
	strcat(message, buf);

        sprintf(buf, "\n\n");
	strcat(message, buf);

        for (i = 10; i > 0; i--)
        {
                sprintf(buf, "[ ] Floor %d: \n", i);
		strcat(message, buf);
        }

	kfree(buf);

        return 0;
}

ssize_t elevator_proc_read(struct file *sp_file, char __user * buf, size_t size, loff_t *offset)
{
        read_p = !read_p;

        if(read_p == 1) return 0;

        printk(KERN_NOTICE "elevator_proc_read called.\n");
        copy_to_user(buf, message, strlen(message));

        return strlen(message);
}

int elevator_proc_release(struct inode *sp_inode, struct file *sp_file)
{
        printk(KERN_NOTICE "elevator_proc_release called.\n");
        kfree(message);

        return 0;
}

static int elevator_init(void)
{
	int i;

	fops.open = elevator_proc_open;
	fops.read = elevator_proc_read;
	fops.release = elevator_proc_release;

        STUB_start_elevator = start_elevator;
        STUB_issue_request = issue_request;
        STUB_stop_elevator = stop_elevator;

	INIT_LIST_HEAD(&elevList);

	for(i = 0; i < 10; i++)
		INIT_LIST_HEAD(&floorLists[i]);

	printk(KERN_NOTICE "Creating proc/elevator.\n");

	if(!proc_create(MODULE_NAME, PERMS, NULL, &fops))
	{
		printk(KERN_NOTICE "Error creating proc/elevator.");
		remove_proc_entry(MODULE_NAME, NULL);
		return -ENOMEM;
	}

        return 0;
}
module_init(elevator_init);

static void elevator_exit(void)
{
        STUB_start_elevator = NULL;
        STUB_issue_request = NULL;
        STUB_stop_elevator = NULL;

	remove_proc_entry(MODULE_NAME, NULL);
	printk(KERN_NOTICE "Removed proc/elevator.\n");

}
module_exit(elevator_exit);
