// Include C library for functions `printf`, `perror`.
#include <stdio.h>
// Include C library for function `exit`.
#include <stdlib.h>
// Include C library for function `wait`.
#include <sys/wait.h>
// Include C library for function `fork`.
#include <unistd.h>

const char* const ERROR_COLLATZ_SEQUENCE_STARTING_NUMBER_IS_NONPOSITIVE = "The given starting number for the Collatz sequence is nonpositive";
const char* const ERROR_FORK_FAILS = "Fork fails";
const char* const ERROR_NUMBER_IN_COLLATZ_SEQUENCE_IS_LESS_THAN_ONE = "A number in the Collatz sequence is less than one";

const int CODE_ERROR_COLLATZ_SEQUENCE_STARTING_NUMBER_IS_NONPOSITIVE = 1;
const int CODE_ERROR_FORK_FAILS = 2;
const int CODE_ERROR_NUMBER_IN_COLLATZ_SEQUENCE_IS_LESS_THAN_ONE = 3;

/* Print the Collatz sequence starting at the number passed in as argument. */
void PrintCollatzSequence( int n )
{
    while ( 1 )
    {
        printf( "%d ", n );
        if ( n < 1 )
        {
            perror( ERROR_NUMBER_IN_COLLATZ_SEQUENCE_IS_LESS_THAN_ONE );
            exit( CODE_ERROR_NUMBER_IN_COLLATZ_SEQUENCE_IS_LESS_THAN_ONE );
        }
        if ( n == 1 )
        {
            break;
        }
        n = n % 2 == 0 ? n / 2 : 3 * n + 1;
    }
    printf( "\n" );
}

int main( int nArgumentsCount, char* kArguments[] )
{
    int nCollatzSequenceStartingNumber = atoi( kArguments[ 1 ] );
    if ( nCollatzSequenceStartingNumber <= 0 )
    {
        perror( ERROR_COLLATZ_SEQUENCE_STARTING_NUMBER_IS_NONPOSITIVE );
        exit( CODE_ERROR_COLLATZ_SEQUENCE_STARTING_NUMBER_IS_NONPOSITIVE );
    }
    pid_t kProcessId = fork();
    if ( kProcessId == 0 )
    {
        printf( "Child process begins.\n" );
        PrintCollatzSequence( nCollatzSequenceStartingNumber );
        printf( "Child process ends.\n" );
    }
    else if ( kProcessId > 0 )
    {
        printf( "Parent process begins waiting.\n" );
        wait( NULL );
        printf( "Parent process ends waiting.\n" );
    }
    else
    {
        perror( ERROR_FORK_FAILS );
        exit( CODE_ERROR_FORK_FAILS );
    }
    return 0;
}
