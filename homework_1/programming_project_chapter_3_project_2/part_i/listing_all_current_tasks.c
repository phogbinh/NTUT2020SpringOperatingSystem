// Include C library for function `printk`; macros `KERN_INFO`, `module_init`, `module_exit`, `MODULE_LICENSE`, `MODULE_DESCRIPTION`, `MODULE_AUTHOR`.
#include <linux/module.h>
// Include C library for structure `task_struct`.
#include <linux/sched.h>
// Include C library for macro `for_each_process`.
#include <linux/sched/signal.h>

/* This function is called when the module is loaded. */
int InitializeListingAllCurrentTasksModule( void )
{
	printk( KERN_INFO "Loading Module\n" );
	struct task_struct* pTask;
	for_each_process( pTask )
	{
		printk( KERN_INFO "Executable name: %s State: %ld Process ID: %d\n", pTask->comm, pTask->state, pTask->pid );
	}
	return 0;
}

/* This function is called when the module is removed. */
void ExitListingAllCurrentTasksModule( void )
{
	printk( KERN_INFO "Removing Module\n" );
}

/* Macros for registering module entry and exit points. */
module_init( InitializeListingAllCurrentTasksModule );
module_exit( ExitListingAllCurrentTasksModule );

MODULE_LICENSE( "MIT" );
MODULE_DESCRIPTION( "Listing All Current Tasks Module" );
MODULE_AUTHOR( "Bill Tran" );