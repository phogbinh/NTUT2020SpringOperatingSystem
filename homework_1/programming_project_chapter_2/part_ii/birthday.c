// Include C library for function `printk`; macros `KERN_INFO`, `module_init`, `module_exit`, `MODULE_LICENSE`, `MODULE_DESCRIPTION`, `MODULE_AUTHOR`.
#include <linux/module.h>
// Include C library for structure `list_head`; macros `LIST_HEAD`, `INIT_LIST_HEAD`, `list_add_tail`, `list_for_each_entry`, `list_for_each_entry_safe`, `list_del`.
#include <linux/types.h>
// Include C library for functions `kmalloc`, `kfree`.
#include <linux/slab.h>

struct birthday
{
    int nDay;
    int nMonth;
    int nYear;
    struct list_head kList;
};

const int FIVE = 5;
static LIST_HEAD( g_kBirthdayListHead );
static const int g_kDates[]  = { 2,    20,   15,   5,    10   };
static const int g_kMonths[] = { 5,    2,    2,    3,    12   };
static const int g_kYears[]  = { 1998, 1995, 1972, 1973, 2003 };

/* Create and insert five objects into the birthday list. */
void CreateAndInsertFiveObjectsIntoBirthdayList( void )
{
    int nIndex;
    for ( nIndex = 0; nIndex < FIVE; nIndex++ )
    {
        struct birthday* pBirthday;
        pBirthday = kmalloc( sizeof( *pBirthday ), GFP_KERNEL );
        pBirthday->nDay   = g_kDates[ nIndex ];
        pBirthday->nMonth = g_kMonths[ nIndex ];
        pBirthday->nYear  = g_kYears[ nIndex ];
        INIT_LIST_HEAD( &( pBirthday->kList ) );
        list_add_tail( &( pBirthday->kList ), &g_kBirthdayListHead );
    }
}

/* Print all objects of the birthday list. */
void PrintAllObjectsOfBirthdayList( void )
{
    struct birthday* pBirthday;
	list_for_each_entry( pBirthday, &g_kBirthdayListHead, kList )
    {
		printk( KERN_INFO "[Printing] Day: %d Month: %d Year: %d\n", pBirthday->nDay, pBirthday->nMonth, pBirthday->nYear );
	}
}

/* This function is called when the module is loaded. */
int InitializeBirthdayModule( void )
{
    printk( KERN_INFO "Loading Module\n" );
    CreateAndInsertFiveObjectsIntoBirthdayList();
    PrintAllObjectsOfBirthdayList();
    return 0;
}

/* Remove all objects from the birthday list. */
void RemoveAllObjectsFromBirthdayList( void )
{
	struct birthday* pBirthday;
    struct birthday* pNext;
	list_for_each_entry_safe( pBirthday, pNext, &g_kBirthdayListHead, kList )
    {
		printk( KERN_INFO "[Removing] Day: %d Month: %d Year: %d\n", pBirthday->nDay, pBirthday->nMonth, pBirthday->nYear );
		list_del( &( pBirthday->kList ) );
		kfree( pBirthday );
	}
}

/* This function is called when the module is removed. */
void ExitBirthdayModule( void )
{
    RemoveAllObjectsFromBirthdayList();
    printk( KERN_INFO "Removing Module\n" );
}

/* Macros for registering module entry and exit points. */
module_init( InitializeBirthdayModule );
module_exit( ExitBirthdayModule );

MODULE_LICENSE( "MIT" );
MODULE_DESCRIPTION( "Birthday Module" );
MODULE_AUTHOR( "Bill Tran" );
