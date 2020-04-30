// Include C library for functions `scanf`, `printf`, `perror`.
#include <stdio.h>
// Include C library for function `exit`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`; type `pthread_t`.
#include <pthread.h>

#define MAXIMUM_FIBONACCI_NUMBERS_COUNT 93

const char* const ERROR_FIBONACCI_NUMBERS_COUNT_IS_INVALID = "The given number of Fibonacci numbers is invalid";

const int CODE_ERROR_FREE = 0;
const int CODE_ERROR_FIBONACCI_NUMBERS_COUNT_IS_INVALID = 1;

long long int g_kFibonacciSequence[ MAXIMUM_FIBONACCI_NUMBERS_COUNT ];

void* GenerateFibonacciSequencePtr( void* pArgument )
{
    const int nFibonacciNumbersCount = *( ( int* )pArgument );
    g_kFibonacciSequence[ 0 ] = 0;
    g_kFibonacciSequence[ 1 ] = 1;
    int nIndex;
    for ( nIndex = 2; nIndex < nFibonacciNumbersCount; nIndex++ )
    {
        g_kFibonacciSequence[ nIndex ] = g_kFibonacciSequence[ nIndex - 1 ] + g_kFibonacciSequence[ nIndex - 2 ];
    }
}

void CreateThreadToGenerateFibonacciSequenceAndWaitForIt( const int nFibonacciNumbersCount )
{
    pthread_t kThreadId;
    pthread_create( &kThreadId, NULL, &GenerateFibonacciSequencePtr, ( void* )&nFibonacciNumbersCount );
    pthread_join( kThreadId, NULL );
}

void PrintFibonacciSequence( const int nFibonacciNumbersCount )
{
    printf( "Generated Fibonacci sequence:\n" );
    int nIndex;
    for ( nIndex = 0; nIndex < nFibonacciNumbersCount; nIndex++ )
    {
        printf( "%lld ", g_kFibonacciSequence[ nIndex ] );
    }
    printf( "\n" );
}

int main( int nArgumentsCount, char* kArguments[] )
{
    int nFibonacciNumbersCount;
    printf( "Enter the number of Fibonacci numbers to generate: " );
    scanf( "%d", &nFibonacciNumbersCount );
    if ( !( 1 <= nFibonacciNumbersCount && nFibonacciNumbersCount <= MAXIMUM_FIBONACCI_NUMBERS_COUNT ) )
    {
        perror( ERROR_FIBONACCI_NUMBERS_COUNT_IS_INVALID );
        exit( CODE_ERROR_FIBONACCI_NUMBERS_COUNT_IS_INVALID );
    }
    CreateThreadToGenerateFibonacciSequenceAndWaitForIt( nFibonacciNumbersCount );
    PrintFibonacciSequence( nFibonacciNumbersCount );
    return CODE_ERROR_FREE;
}