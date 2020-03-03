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

int thread_run(void *data);
void loadElev(void);
void unloadElev(void);

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

struct thread_parameter
{
	struct task_struct *kthread;
	struct mutex mutex;
};

struct thread_parameter elevThread;

char *message;
int read_p;

int elevator_state;
int elevator_animal;
int elevator_direction;
int current_floor;
int num_passengers;
int current_weight;
int passengers_waiting;
int passengers_serviced;
int num_pass_floor[10];
int offload_only;

extern long (*STUB_issue_request)(int, int, int, int);
long issue_request(int num_pets, int pet_type, int start_floor, int destination_floor)
{
	Person *p;

        printk(KERN_NOTICE "System call issue_request called.\n");

	if(num_pets < 0 || num_pets > 3) return 1;
	if(pet_type != 1 && pet_type != 2) return 1;
	if(start_floor < 1 || start_floor > 10) return 1;
	if(destination_floor < 1 || destination_floor > 10) return 1;

	p = kmalloc(sizeof(Person), __GFP_RECLAIM);
	p->num_pets = num_pets;
	p->pet_type = 0;
	if(p->num_pets > 0)
		p->pet_type = pet_type;
	p->start_floor = start_floor-1;
	p->destination_floor = destination_floor-1;

	if(start_floor > destination_floor) p->direction = DOWN;
	else if(start_floor < destination_floor) p->direction = UP;
	else if(start_floor == destination_floor)
	{
		passengers_serviced += 1 + num_pets;
		kfree(p);
		return 0;
	}

	p->weight = 3 + num_pets*pet_type;

	num_pass_floor[p->start_floor] += 1+num_pets;

	passengers_waiting += 1+num_pets;

	list_add_tail(&p->list, &floorLists[p->start_floor]);

        return 0;
}

extern long (*STUB_start_elevator)(void);
long start_elevator(void)
{
        printk(KERN_NOTICE "System call start_elevator called.\n");
	if(elevator_state != OFFLINE)
	{
		return 1;
	}
	else
	{

		elevator_state = IDLE;
		current_floor = 0;
		num_passengers = 0;
		current_weight = 0;
		offload_only = 0;

		return 0;
	}
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void)
{
        printk(KERN_NOTICE "System call stop_elevator.\n");

	if (offload_only == 0)
	{
		offload_only = 1;
		return 0;
	}


        return 1;
}

int elevator_proc_open(struct inode *sp_inode, struct file *sp_file)
{
        int i;
	int j;
	char * buf;

	Person *p;

	struct list_head *temp;

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

	if(elevator_state == OFFLINE)
	{
        	sprintf(buf, "Elevator State: %s\n", "OFFLINE");
		strcat(message, buf);
	}
	else if(elevator_state == IDLE)
        {
                sprintf(buf, "Elevator State: %s\n", "IDLE");
                strcat(message, buf);
        }
        else if(elevator_state == LOADING)
        {
                sprintf(buf, "Elevator State: %s\n", "LOADING");
                strcat(message, buf);
        }
        else if(elevator_state == UP)
        {
                sprintf(buf, "Elevator State: %s\n", "UP");
                strcat(message, buf);
        }
        else if(elevator_state == DOWN)
        {
                sprintf(buf, "Elevator State: %s\n", "DOWN");
                strcat(message, buf);
        }

	if(elevator_animal == CAT_TYPE)
	{
        	sprintf(buf, "Elevator Animals: %s\n", "CAT");
		strcat(message, buf);
	}
        else if(elevator_animal == DOG_TYPE)
        {
                sprintf(buf, "Elevator Animals: %s\n", "DOG");
                strcat(message, buf);
        }
	else if(elevator_animal == 0)
	{
                sprintf(buf, "Elevator Animals: %s\n", "NONE");
                strcat(message, buf);
	}

        sprintf(buf, "Current Floor: %d\n", (current_floor+1));
	strcat(message, buf);
        sprintf(buf, "Number of Passengers: %d\n", num_passengers);
	strcat(message, buf);
        sprintf(buf, "Current Weight: %d\n", current_weight);
	strcat(message, buf);
        sprintf(buf, "Number of Passengers Waiting: %d\n", passengers_waiting);
	strcat(message, buf);
        sprintf(buf, "Number of Passengers Serviced %d\n", passengers_serviced);
	strcat(message, buf);

        sprintf(buf, "\n\n");
	strcat(message, buf);

        for (i = 10; i > 0; i--)
        {
		if(current_floor == i-1)
                	sprintf(buf, "[*] Floor %d: %d", i, num_pass_floor[i-1]);
		else
			sprintf(buf, "[ ] Floor %d: %d", i, num_pass_floor[i-1]);

		strcat(message, buf);

		list_for_each(temp, &floorLists[i-1])
		{
			p = list_entry(temp, Person, list);

			sprintf(buf, " |");
			strcat(message, buf);

			for(j = 0; j < p->num_pets; j++)
			{
				if(p->pet_type == DOG_TYPE)
				{
					sprintf(buf, " x");
					strcat(message, buf);
				}
				else if(p->pet_type == CAT_TYPE)
				{
					sprintf(buf, " o");
					strcat(message, buf);
				}
			}
		}

		sprintf(buf, "\n");
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

void thread_init_parameter(struct thread_parameter *parm)
{
	mutex_init(&parm->mutex);
	parm->kthread = kthread_run(thread_run, parm, "elevator thread.");
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

	elevator_state = OFFLINE;
	elevator_animal = 0;
	current_floor = 0;
	num_passengers = 0;
	current_weight = 0;
	passengers_waiting = 0;
	passengers_serviced = 0;

	for(i = 0; i < 10; i++)
		num_pass_floor[i] = 0;

	INIT_LIST_HEAD(&elevList);

	for(i = 0; i < 10; i++)
		INIT_LIST_HEAD(&floorLists[i]);

	thread_init_parameter(&elevThread);
	if(IS_ERR(elevThread.kthread))
	{
		printk(KERN_WARNING "ERROR: elevator thread.\n");
		remove_proc_entry(MODULE_NAME, NULL);
		return PTR_ERR(elevThread.kthread);
	}

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

	kthread_stop(elevThread.kthread);
	remove_proc_entry(MODULE_NAME, NULL);
	mutex_destroy(&elevThread.mutex);

	printk(KERN_NOTICE "Removed proc/elevator.\n");

}
module_exit(elevator_exit);


int thread_run(void *data)
{
	int i;

	struct thread_parameter *parm = data;

	while(!kthread_should_stop())
	{

		if(mutex_lock_interruptible(&parm->mutex)==0)
		{
			printk(KERN_NOTICE "Might be working?");

			while(elevator_state != OFFLINE)
			{
				for(i = 0; i < 10; i++)
				{
					elevator_state = UP;
					elevator_direction = UP;

					elevator_state = LOADING;
					if(current_weight != 0)
						unloadElev();

                        		if (offload_only == 1 && current_weight == 0)
                        		{
                                		elevator_state = OFFLINE;
                                		break;
                        		}

					printk(KERN_NOTICE "this is between unload and load on up.\n");

					loadElev();

					printk(KERN_NOTICE "this is after load on up.\n");

					elevator_state = UP;

					ssleep(2);

					if(i != 9)
						current_floor++;
				}

				if(elevator_state == OFFLINE) break;

				for (i = 10; i > 0; i--)
				{
					elevator_state = DOWN;
					elevator_direction = DOWN;

					elevator_state = LOADING;
					if(current_weight != 0)
						unloadElev();

                        		if (offload_only == 1 && current_weight == 0)
                        		{
                                		elevator_state = OFFLINE;
                                		break;
                        		}

					printk(KERN_NOTICE "this is between unload and load on down.\n");

					loadElev();

					printk(KERN_NOTICE "this is after load on down.\n");

					elevator_state = DOWN;

					ssleep(2);

					if(i != 1)
						current_floor--;
				}
			}
		}
		mutex_unlock(&parm->mutex);
	}
	return 0;
}


void loadElev(void)
{
	struct list_head *temp;
	struct list_head *dummy;

	Person *p;

	list_for_each_safe(temp, dummy, &floorLists[current_floor])
	{
		p = list_entry(temp, Person, list);

		if(current_weight+p->weight <= 15)
		{
			if(p->pet_type == elevator_animal || elevator_animal == 0)
			{
				if(p->direction == elevator_direction)
				{
					num_pass_floor[current_floor] -= (1 + p->num_pets);

					elevator_animal = p->pet_type;

                                        passengers_waiting -= (1+p->num_pets);
                                        current_weight += p->weight;

					num_passengers += (1+p->num_pets);

					ssleep(1);

                                        list_move_tail(temp, &elevList);
				}
			}
		}
	}
}

void unloadElev(void)
{
	struct list_head *temp;
	struct list_head *dummy;
       	Person *p;

	list_for_each_safe(temp, dummy, &elevList)
	{
		p = list_entry(temp, Person, list);

		if(p->destination_floor == current_floor)
		{
			current_weight -= p->weight;
			num_passengers -= (1+p->num_pets);
			passengers_serviced += (1+p->num_pets);

			ssleep(1);

			list_del(temp);
			kfree(p);
		}
	}

	if(current_weight == num_passengers*3)
	{
		elevator_animal = 0;
	}
}

