// Include C library for function `printk`; macros `KERN_INFO`, `module_init`, `module_exit`, `MODULE_LICENSE`, `MODULE_DESCRIPTION`, `MODULE_AUTHOR`.
#include <linux/module.h>

/* This function is called when the module is loaded. */
int simple_init( void )
{
    printk( KERN_INFO "Loading Module\n" );
    return 0;
}

/* This function is called when the module is removed. */
void simple_exit( void )
{
    printk( KERN_INFO "Removing Module\n" );
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "Simple Module" );
MODULE_AUTHOR( "SGG" );