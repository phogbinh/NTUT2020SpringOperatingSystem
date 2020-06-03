#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

///
/// Types
///
typedef enum
{
    false = 0,
    true
} bool;

///
/// Macros
///
#define CUSTOMER_THREADS_COUNT 5
#define RESOURCE_TYPES_COUNT 3

///
/// Global variable
///
pthread_mutex_t g_kMutex;
int g_kResourceTypesAvailableQuantities[ RESOURCE_TYPES_COUNT ];
int g_kCustomerThreadResourceTypeRequestMaximumQuantity[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
int g_kCustomerThreadResourceTypeAllocatedQuantity[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
int g_kCustomerThreadResourceTypeNeedQuantity[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
bool g_kIsCompleteRequestingAndReleasingResources[ CUSTOMER_THREADS_COUNT ];
int g_kSafeSequence[ CUSTOMER_THREADS_COUNT ];

///
/// Functions' declarations
///
void InitializeIsCompleteRequestingAndReleasingResources();
void InitializeResourceTypesAvailableQuantities( char* const * const pInputResourceTypesQuantities );
void InitializeCustomerThreadResourceTypeRequestMaximumQuantity();
void InitializeCustomerThreadResourceTypeAllocatedQuantity();
void InitializeCustomerThreadResourceTypeNeedQuantity();
void PrintResourceTypesAndTheirAvailableQuantities();
void PrintInitializedCustomerThreadResourceTypeRequestMaximumQuantity();
void PrintCustomerThreadResourceTypeMatrix( int kMatrix[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ] );
void PrintCurrentState();
void CreateCustomerThreadsToRequestAndReleaseResourcesAndWaitForThem();
void* RequestAndReleaseResourcesPtr( void* pArgument );
bool IsSuccessfulRequestingResources(int customer_num, int request[]);
void PrintCustomerThreadRequest( const int nCustomerThreadIndex, int kResourceTypesRequestQuantities[] );
bool IsSufficientResourceTypesAvailableQuantitiesToFulfillRequest( int kResourceTypesRequestQuantities[] );
bool IsFoundSafeSequence(int customer_num,int request[]);
void PrintFoundSafeSequence();
void FulfillRequest( const int nCustomerThreadIndex, int kResourceTypesRequestQuantities[] );
void SetIsCompleteRequestingAndReleasingResource( const int nCustomerThreadIndex );
void ReleaseResources( const int nCustomerThreadIndex );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    InitializeIsCompleteRequestingAndReleasingResources();
    InitializeResourceTypesAvailableQuantities( kArguments + 1 );
    InitializeCustomerThreadResourceTypeRequestMaximumQuantity();
    InitializeCustomerThreadResourceTypeAllocatedQuantity();
    InitializeCustomerThreadResourceTypeNeedQuantity();
    PrintResourceTypesAndTheirAvailableQuantities();
    printf( "\n" );
    PrintInitializedCustomerThreadResourceTypeRequestMaximumQuantity();
    PrintCurrentState();
    CreateCustomerThreadsToRequestAndReleaseResourcesAndWaitForThem();
    return 0;
}

///
/// Functions' definitions
///
void InitializeIsCompleteRequestingAndReleasingResources()
{
    int nIndex;
    for ( nIndex = 0; nIndex < CUSTOMER_THREADS_COUNT; nIndex++ )
    {
        g_kIsCompleteRequestingAndReleasingResources[ nIndex ] = false;
    }
}

void InitializeResourceTypesAvailableQuantities( char* const * const pInputResourceTypesQuantities )
{
    int nIndex;
    for ( nIndex = 0; nIndex < RESOURCE_TYPES_COUNT; nIndex++ )
    {
        g_kResourceTypesAvailableQuantities[ nIndex ] = atoi( *( pInputResourceTypesQuantities + nIndex ) );
    }
}

void InitializeCustomerThreadResourceTypeRequestMaximumQuantity()
{
    int nCustomerThreadIndex;
    int nResourceTypeIndex;
    for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
    {
        for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            g_kCustomerThreadResourceTypeRequestMaximumQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] = rand() % ( g_kResourceTypesAvailableQuantities[ nResourceTypeIndex ] + 1 );
        }
    }
}

void InitializeCustomerThreadResourceTypeAllocatedQuantity()
{
    int nCustomerThreadIndex;
    int nResourceTypeIndex;
    for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
    {
        for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            g_kCustomerThreadResourceTypeAllocatedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] = 0;
        }
    }
}

void InitializeCustomerThreadResourceTypeNeedQuantity()
{
    int nCustomerThreadIndex;
    int nResourceTypeIndex;
    for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
    {
        for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            g_kCustomerThreadResourceTypeNeedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] = g_kCustomerThreadResourceTypeRequestMaximumQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ];
        }
    }
}

void PrintResourceTypesAndTheirAvailableQuantities()
{
    printf( "Resource types and their available quantities:\n" );
    int nIndex;
    for ( nIndex = 0; nIndex < RESOURCE_TYPES_COUNT; nIndex++ )
    {
        printf( "%c ", 'A' + nIndex );
    }
    printf( "\n" );
    for ( nIndex = 0; nIndex < RESOURCE_TYPES_COUNT; nIndex++ )
    {
        printf( "%d ", g_kResourceTypesAvailableQuantities[ nIndex ] );
    }
    printf( "\n" );
}

void PrintInitializedCustomerThreadResourceTypeRequestMaximumQuantity()
{
    printf( "Initialized (randomly generated) [customer thread - resource type] request's maximum quantity:\n" );
    PrintCustomerThreadResourceTypeMatrix( g_kCustomerThreadResourceTypeRequestMaximumQuantity );
}

void PrintCustomerThreadResourceTypeMatrix( int kMatrix[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ] )
{
    int nCustomerThreadIndex;
    int nResourceTypeIndex;
    for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
    {
        printf( "Customer thread #%d    ", nCustomerThreadIndex );
        for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            printf( "%d ", kMatrix[ nCustomerThreadIndex ][ nResourceTypeIndex ] );
        }
        printf( "\n" );
    }
}

void PrintCurrentState()
{
    printf( "\n** CURRENT STATE BEGIN **\n" );
    printf( "Current [customer thread - resource type] allocated quantity:\n" );
    PrintCustomerThreadResourceTypeMatrix( g_kCustomerThreadResourceTypeAllocatedQuantity );
    printf( "Current [customer thread - resource type] need quantity:\n" );
    PrintCustomerThreadResourceTypeMatrix( g_kCustomerThreadResourceTypeNeedQuantity );
    PrintResourceTypesAndTheirAvailableQuantities();
    printf( "** CURRENT STATE END **\n" );
}

void CreateCustomerThreadsToRequestAndReleaseResourcesAndWaitForThem()
{
    pthread_t kCustomerThreadsIds[ CUSTOMER_THREADS_COUNT ];
    const int kCustomerThreadsIndices[] = { 0, 1, 2, 3, 4 };
    int nIndex;
    pthread_mutex_init( &g_kMutex, NULL );
    for ( nIndex = 0; nIndex < CUSTOMER_THREADS_COUNT; nIndex++ )
    {
        pthread_create( &kCustomerThreadsIds[ nIndex ], NULL, &RequestAndReleaseResourcesPtr, ( void* )&kCustomerThreadsIndices[ nIndex ] );
    }
    for ( nIndex = 0; nIndex < CUSTOMER_THREADS_COUNT; nIndex++ )
    {
        pthread_join( kCustomerThreadsIds[ nIndex ], NULL );
    }
    pthread_mutex_destroy( &g_kMutex );
}

void* RequestAndReleaseResourcesPtr( void *pArgument )
{
    const int nCustomerThreadIndex = *( ( int* ) pArgument );
    while ( !g_kIsCompleteRequestingAndReleasingResources[ nCustomerThreadIndex ] )
    {
        int kResourceTypesRequestQuantities[ RESOURCE_TYPES_COUNT ];
        int nResourcesRequestQuantitiesCount = 0;
        for ( int nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            kResourceTypesRequestQuantities[ nResourceTypeIndex ] = rand() % ( g_kCustomerThreadResourceTypeNeedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] + 1 );
            nResourcesRequestQuantitiesCount += kResourceTypesRequestQuantities[ nResourceTypeIndex ];
        }
        if ( nResourcesRequestQuantitiesCount > 0 )
        {
            while ( !IsSuccessfulRequestingResources( nCustomerThreadIndex, kResourceTypesRequestQuantities ) );
        }
    }
}

bool IsSuccessfulRequestingResources( const int nCustomerThreadIndex, int kResourceTypesRequestQuantities[] )
{
    pthread_mutex_lock( &g_kMutex );
    PrintCustomerThreadRequest( nCustomerThreadIndex, kResourceTypesRequestQuantities );
    if ( !IsSufficientResourceTypesAvailableQuantitiesToFulfillRequest( kResourceTypesRequestQuantities ) )
    {
        printf( "There is at least one insufficent resource types available quantities to fulfill the request of customer thread #%d.\n", nCustomerThreadIndex );
        pthread_mutex_unlock( &g_kMutex );
        return false;
    }
    bool bIsFoundSafeSequence = IsFoundSafeSequence( nCustomerThreadIndex, kResourceTypesRequestQuantities );
    if ( !bIsFoundSafeSequence )
    {
        printf( "A safe sequence cannot be found to fulfill the request of customer thread #%d.\n", nCustomerThreadIndex );
        pthread_mutex_unlock( &g_kMutex );
        return false;
    }
    PrintFoundSafeSequence();
    FulfillRequest( nCustomerThreadIndex, kResourceTypesRequestQuantities );
    SetIsCompleteRequestingAndReleasingResource( nCustomerThreadIndex );
    if (g_kIsCompleteRequestingAndReleasingResources[nCustomerThreadIndex])
    {
        ReleaseResources(nCustomerThreadIndex);
    }
    PrintCurrentState();
    pthread_mutex_unlock( &g_kMutex );
    return true;
}

void PrintCustomerThreadRequest( const int nCustomerThreadIndex, int kResourceTypesRequestQuantities[] )
{
    printf( "Customer thread #%d requests ", nCustomerThreadIndex );
    int nIndex;
    for ( nIndex = 0; nIndex < RESOURCE_TYPES_COUNT; nIndex++ )
    {
        printf( "%d ", kResourceTypesRequestQuantities[ nIndex ] );
    }
    printf( "\n" );
}

bool IsSufficientResourceTypesAvailableQuantitiesToFulfillRequest( int kResourceTypesRequestQuantities[] )
{
    int nIndex;
    for ( nIndex = 0; nIndex < RESOURCE_TYPES_COUNT; nIndex++ )
    {
        if ( g_kResourceTypesAvailableQuantities[ nIndex ] < kResourceTypesRequestQuantities[ nIndex ] )
        {
            return false;
        }
    }
    return true;
}

bool IsFoundSafeSequence( const int nCurrentCustomerThreadIndex, int kResourceTypesRequestQuantities[] )
{
    int kBankerAvailable[ RESOURCE_TYPES_COUNT ];
    int kBankerMax[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
    int kBankerAllocation[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
    int kBankerNeed[ CUSTOMER_THREADS_COUNT ][ RESOURCE_TYPES_COUNT ];
    /* Initialization */
    int nResourceTypeIndex;
    for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
    {
        kBankerAvailable[ nResourceTypeIndex ] = g_kResourceTypesAvailableQuantities[ nResourceTypeIndex ];
    }
    int nCustomerThreadIndex;
    for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
    {
        for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
        {
            kBankerAllocation[ nCustomerThreadIndex ][ nResourceTypeIndex ] = g_kCustomerThreadResourceTypeAllocatedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ];
            kBankerMax[ nCustomerThreadIndex ][ nResourceTypeIndex ] = g_kCustomerThreadResourceTypeRequestMaximumQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ];
            kBankerNeed[ nCustomerThreadIndex ][ nResourceTypeIndex ] = g_kCustomerThreadResourceTypeNeedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ];
        }
    }
    for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
    {
        kBankerAvailable[ nResourceTypeIndex ] -= kResourceTypesRequestQuantities[ nResourceTypeIndex ];
        kBankerAllocation[ nCurrentCustomerThreadIndex ][ nResourceTypeIndex ] += kResourceTypesRequestQuantities[ nResourceTypeIndex ];
        kBankerNeed[ nCurrentCustomerThreadIndex ][ nResourceTypeIndex ] -= kResourceTypesRequestQuantities[ nResourceTypeIndex ];
    }
    /* Safety Algorithm */
    bool kIsComplete[ CUSTOMER_THREADS_COUNT ] = { false };
    int nCount = 0;
    while ( true )
    {
        int nTemporaryCustomerThreadIndex = -1;
        for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++ )
        {
            bool bIsNeedLessThanAvailable = true;
            for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
            {
                if ( kBankerNeed[ nCustomerThreadIndex ][ nResourceTypeIndex ] > kBankerAvailable[ nResourceTypeIndex ] || kIsComplete[ nCustomerThreadIndex ] )
                {
                    bIsNeedLessThanAvailable = false;
                    break;
                }
            }
            if ( bIsNeedLessThanAvailable )
            {
                nTemporaryCustomerThreadIndex = nCustomerThreadIndex;
                break;
            }
        }

        if ( nTemporaryCustomerThreadIndex != -1 )
        {
            g_kSafeSequence[ nCount ] = nTemporaryCustomerThreadIndex;
            nCount++;
            kIsComplete[ nTemporaryCustomerThreadIndex ] = true;
            for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
            {
                kBankerAvailable[ nResourceTypeIndex ] += kBankerAllocation[nTemporaryCustomerThreadIndex][ nResourceTypeIndex ];
            }
        }
        else
        {
            for ( nCustomerThreadIndex = 0; nCustomerThreadIndex < CUSTOMER_THREADS_COUNT; nCustomerThreadIndex++)
            {
                if ( !kIsComplete[ nCustomerThreadIndex ] )
                {
                    return false;
                }
            }
            return true;
        }
    }
}

void PrintFoundSafeSequence()
{
    printf( "A safe sequence for the customer threads is found: " );
    int nIndex;
    for ( nIndex = 0; nIndex < CUSTOMER_THREADS_COUNT; nIndex++ )
    {
        printf( "%d ", g_kSafeSequence[ nIndex ] );
    }
    printf( "\n" );
}

void FulfillRequest( const int nCustomerThreadIndex, int kResourceTypesRequestQuantities[] )
{
    int nResourceTypeIndex;
    for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
    {
        g_kCustomerThreadResourceTypeAllocatedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] += kResourceTypesRequestQuantities[ nResourceTypeIndex ];
        g_kResourceTypesAvailableQuantities[ nResourceTypeIndex ] -= kResourceTypesRequestQuantities[ nResourceTypeIndex ];
        g_kCustomerThreadResourceTypeNeedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] -= kResourceTypesRequestQuantities[ nResourceTypeIndex ];
    }
}

void SetIsCompleteRequestingAndReleasingResource( const int nCustomerThreadIndex )
{
    int nResourceTypeIndex;
    for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
    {
        if ( g_kCustomerThreadResourceTypeNeedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] > 0 )
        {
            g_kIsCompleteRequestingAndReleasingResources[ nCustomerThreadIndex ] = false;
            return;
        }
    }
    g_kIsCompleteRequestingAndReleasingResources[ nCustomerThreadIndex ] = true;
}

void ReleaseResources( const int nCustomerThreadIndex )
{
    printf( "Customer thread #%d releases all resources.\n", nCustomerThreadIndex );
    int nResourceTypeIndex;
    for ( nResourceTypeIndex = 0; nResourceTypeIndex < RESOURCE_TYPES_COUNT; nResourceTypeIndex++ )
    {
        g_kResourceTypesAvailableQuantities[ nResourceTypeIndex ] += g_kCustomerThreadResourceTypeAllocatedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ];
        g_kCustomerThreadResourceTypeAllocatedQuantity[ nCustomerThreadIndex ][ nResourceTypeIndex ] = 0;
    }
}