#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

#include "systemcalls.h"

long (*STUB_start_elevator)(void);
EXPORT_SYMBOL(STUB_start_elevator);

SYSCALL_DEFINE0(start_elevator)
{
        printk(KERN_NOTICE "Inside SYSCALL_DEIFINE0 block. %s\n", __FUNCTION__);

        if(STUB_start_elevator != NULL)
                return STUB_start_elevator();
        else
                return -ENOSYS;
}
