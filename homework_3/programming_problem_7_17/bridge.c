// Include C library for functions `scanf`, `printf`, `perror`.
#include <stdio.h>
// Include C library for functions `srand`, `rand`, `time`, `exit`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`; types `pthread_t`, `pthread_mutex_t`.
#include <pthread.h>
// Include C library for function `sleep`.
#include <unistd.h>

///
/// Types
///
typedef enum
{
    north = 0,
    south
} direction;

typedef struct
{
    int nRegionalId;
    direction kDirection;
} farmer;

///
/// Macros
///
#define MAXIMUM_CROSSING_TIME 3
#define MAXIMUM_FARMERS_COUNT 10000

///
/// Constants
///
const char* DIRECTIONS[ 2 ] = { "North", "South" };

const char* const INSTRUCTION_INPUT_NUMBER_OF_NORTH_FARMERS = "Enter the number of North farmers";
const char* const INSTRUCTION_INPUT_NUMBER_OF_SOUTH_FARMERS = "Enter the number of South farmers";

const char* const ERROR_INPUT_NUMBER_IS_NON_POSITIVE = "The given input number is non positive";

const int CODE_ERROR_FREE                         = 0;
const int CODE_ERROR_INPUT_NUMBER_IS_NON_POSITIVE = 1;

///
/// Global variables
///
pthread_mutex_t g_kMutex;

///
/// Functions' declarations
///
void InputAndInitializeFarmers( farmer* const pFarmers, int* const pFarmersCount );
void InputPositiveInteger( int* const pInputNumber, const char* const pInputInstruction );
void InitializeRegionalFarmers( const direction kRegionalFarmerDirection, const int nRegionalFarmersCount, const int nRegionalFarmersStartingIndexInFarmers, farmer* const pFarmers );
void CreateThreadsAsFarmersToCrossBridgeAndWaitForThem( farmer* const pFarmers, const int nFarmersCount );
void* CrossBridgePtr( void* pArgument );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    srand( time( NULL ) );
    farmer kFarmers[ MAXIMUM_FARMERS_COUNT ];
    int nFarmersCount;
    InputAndInitializeFarmers( kFarmers, &nFarmersCount );
    CreateThreadsAsFarmersToCrossBridgeAndWaitForThem( kFarmers, nFarmersCount );
    return CODE_ERROR_FREE;
}

///
/// Functions' definitions
///
void InputAndInitializeFarmers( farmer* const pFarmers, int* const pFarmersCount )
{
    int nNorthFarmersCount;
    int nSouthFarmersCount;
    InputPositiveInteger( &nNorthFarmersCount, INSTRUCTION_INPUT_NUMBER_OF_NORTH_FARMERS );
    InputPositiveInteger( &nSouthFarmersCount, INSTRUCTION_INPUT_NUMBER_OF_SOUTH_FARMERS );
    InitializeRegionalFarmers( north, nNorthFarmersCount, 0                 , pFarmers );
    InitializeRegionalFarmers( south, nSouthFarmersCount, nNorthFarmersCount, pFarmers );
    *pFarmersCount = nNorthFarmersCount + nSouthFarmersCount;
}

void InputPositiveInteger( int* const pInputNumber, const char* const pInputInstruction )
{
    printf( "%s: ", pInputInstruction );
    scanf( "%d", pInputNumber );
    if ( *pInputNumber <= 0 )
    {
        perror( ERROR_INPUT_NUMBER_IS_NON_POSITIVE );
        exit( CODE_ERROR_INPUT_NUMBER_IS_NON_POSITIVE );
    }
}

void InitializeRegionalFarmers( const direction kRegionalFarmerDirection, const int nRegionalFarmersCount, const int nRegionalFarmersStartingIndexInFarmers, farmer* const pFarmers )
{
    int nIndex;
    int nFarmerRegionalId = 0;
    for ( nIndex = nRegionalFarmersStartingIndexInFarmers; nIndex < nRegionalFarmersStartingIndexInFarmers + nRegionalFarmersCount; nIndex++ )
    {
        ( pFarmers + nIndex )->nRegionalId = nFarmerRegionalId++;
        ( pFarmers + nIndex )->kDirection = kRegionalFarmerDirection;
    }
}

void CreateThreadsAsFarmersToCrossBridgeAndWaitForThem( farmer* const pFarmers, const int nFarmersCount )
{
    pthread_t kThreads[ nFarmersCount ];
    int nIndex;
    pthread_mutex_init( &g_kMutex, NULL );
    for ( nIndex = 0; nIndex < nFarmersCount; nIndex++ )
    {
        pthread_create( &kThreads[ nIndex ], NULL, CrossBridgePtr, pFarmers + nIndex );
    }
    for ( nIndex = 0; nIndex < nFarmersCount; nIndex++ )    
    {
        pthread_join( kThreads[ nIndex ], NULL );
    }
    pthread_mutex_destroy( &g_kMutex );
}

void* CrossBridgePtr( void* pArgument )
{
    farmer* pFarmer = ( farmer* )pArgument;
    printf( "%s farmer %d requests permission to enter the bridge.\n", DIRECTIONS[ pFarmer->kDirection ], pFarmer->nRegionalId );
    pthread_mutex_lock( &g_kMutex );
    int nCrossingTime = rand() % MAXIMUM_CROSSING_TIME + 1;
    printf( "%s farmer %d was given permission and will cross the bridge in %d seconds.\n", DIRECTIONS[ pFarmer->kDirection ], pFarmer->nRegionalId, nCrossingTime );
    sleep( nCrossingTime );
    printf( "%s farmer %d crossed the bridge.\n", DIRECTIONS[ pFarmer->kDirection ], pFarmer->nRegionalId );
    pthread_mutex_unlock( &g_kMutex );
}