// Include C library for function `printk`; macros `KERN_INFO`, `module_init`, `module_exit`, `MODULE_LICENSE`, `MODULE_DESCRIPTION`, `MODULE_AUTHOR`.
#include <linux/module.h>
// Include C library for structure `task_struct`.
#include <linux/sched.h>
// Include C library for variable `init_task`.
#include <linux/sched/task.h>
// Include C library for structure `list_head`; macros `list_for_each`, `list_entry`.
#include <linux/types.h>

/* List all current tasks using Depth-First Search (DFS) algorithm. */
void ListTasksUsingDFS( struct task_struct* pTask )
{
	struct list_head* pList;
	list_for_each( pList, &pTask->children )
	{
		struct task_struct* pNextTask = list_entry( pList, struct task_struct, sibling );
		printk( KERN_INFO "Executable name: %s State: %ld Process ID: %d\n", pNextTask->comm, pNextTask->state, pNextTask->pid );
		ListTasksUsingDFS( pNextTask );
	}
}

/* This function is called when the module is loaded. */
int InitializeDFSListingAllCurrentTasksModule( void )
{
	printk( KERN_INFO "Loading Module\n" );
	ListTasksUsingDFS( &init_task );
	return 0;
}

/* This function is called when the module is removed. */
void ExitDFSListingAllCurrentTasksModule( void )
{
	printk( KERN_INFO "Removing Module\n" );
}

/* Macros for registering module entry and exit points. */
module_init( InitializeDFSListingAllCurrentTasksModule );
module_exit( ExitDFSListingAllCurrentTasksModule );

MODULE_LICENSE( "MIT" );
MODULE_DESCRIPTION( "DFS Listing All Current Tasks Module" );
MODULE_AUTHOR( "Bill Tran" );