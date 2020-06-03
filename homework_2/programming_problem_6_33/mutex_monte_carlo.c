// Include C library for functions `scanf`, `printf`, `perror`.
#include <stdio.h>
// Include C library for functions `srand`, `rand`, `time`, `exit`; macro `RAND_MAX`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`; types `pthread_t`, `pthread_mutex_t`.
#include <pthread.h>

///
/// Type
///
typedef enum
{
    false = 0,
    true
} bool;

///
/// Macro
///
#define MAXIMUM_THREADS_COUNT 50

///
/// Constants
///
const char* const ERROR_INPUT_NUMBER_IS_NON_POSITIVE = "The input number is non-positive";

const char* const INSTRUCTION_INPUT_NUMBER_OF_POINTS = "Enter the number of points to generate";
const char* const INSTRUCTION_INPUT_NUMBER_OF_THREADS = "Enter the number of threads to create";

const int CODE_ERROR_FREE = 0;
const int CODE_ERROR_INPUT_NUMBER_IS_NON_POSITIVE = 1;

///
/// Global variables
///
int g_nPointsInUnitCircleCount;
pthread_mutex_t g_kMutex;

///
/// Functions' declarations
///
void InputPositiveInteger( int* pInputNumber, const char* const pInputInstruction );
void CreateThreadsToCountNumberOfPointsInUnitCircleAndWaitForThem( const int nPointsCount, const int nThreadsCount );
void* CountNumberOfPointsInUnitCirclePtr( void* pArgument );
bool IsInUnitCircle( const double fPointX, const double fPointY );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    srand( time( NULL ) );
    int nPointsCount;
    InputPositiveInteger( &nPointsCount, INSTRUCTION_INPUT_NUMBER_OF_POINTS );
    int nThreadsCount;
    InputPositiveInteger( &nThreadsCount, INSTRUCTION_INPUT_NUMBER_OF_THREADS );
    g_nPointsInUnitCircleCount = 0;
    CreateThreadsToCountNumberOfPointsInUnitCircleAndWaitForThem( nPointsCount, nThreadsCount );
    printf( "Estimated pi: %f\n", 4.0 * g_nPointsInUnitCircleCount / nPointsCount );
    return CODE_ERROR_FREE;
}

///
/// Functions' definitions
///
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

void CreateThreadsToCountNumberOfPointsInUnitCircleAndWaitForThem( const int nPointsCount, const int nThreadsCount )
{
    const int nPointsPerThreadCount = nPointsCount / nThreadsCount;
    pthread_t kThreadsIds[ MAXIMUM_THREADS_COUNT ];
    int nIndex;
    pthread_mutex_init( &g_kMutex, NULL );
    for ( nIndex = 0; nIndex < nThreadsCount; nIndex++ )
    {
        pthread_create( &kThreadsIds[ nIndex ], NULL, &CountNumberOfPointsInUnitCirclePtr, ( void* )&nPointsPerThreadCount );
    }
    for ( nIndex = 0; nIndex < nThreadsCount; nIndex++ )
    {
        pthread_join( kThreadsIds[ nIndex ], NULL );
    }
    pthread_mutex_destroy( &g_kMutex );
}

void* CountNumberOfPointsInUnitCirclePtr( void* pArgument )
{
    const int nThreadPointsCount = *( ( int* )pArgument );
    int nThreadPointsInUnitCircleCount = 0;
    int nIndex;
    for ( nIndex = 0; nIndex < nThreadPointsCount; nIndex++ )
    {
        double fPointX = ( double )rand() / RAND_MAX;
        double fPointY = ( double )rand() / RAND_MAX;
        if ( IsInUnitCircle( fPointX, fPointY ) )
        {
            nThreadPointsInUnitCircleCount++;
        }
    }
    pthread_mutex_lock( &g_kMutex );
    g_nPointsInUnitCircleCount += nThreadPointsInUnitCircleCount;
    pthread_mutex_unlock( &g_kMutex );
}

bool IsInUnitCircle( const double fPointX, const double fPointY )
{
    return fPointX * fPointX + fPointY * fPointY <= 1;
}
