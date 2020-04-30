// Include C library for functions `scanf`, `printf`, `perror`.
#include <stdio.h>
// Include C library for functions `srand`, `rand`, `time`, `exit`; macro `RAND_MAX`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`; type `pthread_t`.
#include <pthread.h>

typedef enum
{
    false = 0,
    true
} bool;

const char* const ERROR_POINTS_COUNT_IS_NON_POSITIVE = "The given number of points is non-positive";

const int CODE_ERROR_FREE = 0;
const int CODE_ERROR_POINTS_COUNT_IS_NON_POSITIVE = 1;

int g_nPointsInUnitCircleCount;

bool IsInUnitCircle( const double fPointX, const double fPointY )
{
    return fPointX * fPointX + fPointY * fPointY <= 1;
}

void* CountNumberOfPointsInUnitCirclePtr( void* pArgument )
{
    const int nPointsCount = *( ( int* )pArgument );
    g_nPointsInUnitCircleCount = 0;
    int nIndex;
    for ( nIndex = 0; nIndex < nPointsCount; nIndex++ )
    {
        double fPointX = ( double )rand() / RAND_MAX;
        double fPointY = ( double )rand() / RAND_MAX;
        if ( IsInUnitCircle( fPointX, fPointY ) )
        {
            g_nPointsInUnitCircleCount++;
        }
    }
}

void CreateThreadsToCountNumberOfPointsInUnitCircleAndWaitForThem( const int nPointsCount )
{
    pthread_t kThreadId;
    pthread_create( &kThreadId, NULL, &CountNumberOfPointsInUnitCirclePtr, ( void* )&nPointsCount );
    pthread_join( kThreadId, NULL );
}

int main( int nArgumentsCount, char* kArguments[] )
{
    srand( time( NULL ) );
    int nPointsCount;
    printf( "Enter the number of points to generate: " );
    scanf( "%d", &nPointsCount );
    if ( nPointsCount <= 0 )
    {
        perror( ERROR_POINTS_COUNT_IS_NON_POSITIVE );
        exit( CODE_ERROR_POINTS_COUNT_IS_NON_POSITIVE );
    }
    CreateThreadsToCountNumberOfPointsInUnitCircleAndWaitForThem( nPointsCount );
    printf( "Estimated pi: %f\n", 4.0 * g_nPointsInUnitCircleCount / nPointsCount );
    return CODE_ERROR_FREE;
}