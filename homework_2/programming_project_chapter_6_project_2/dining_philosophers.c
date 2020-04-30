// Include C library for function `printf`.
#include <stdio.h>
// Include C library for functions `srand`, `rand`, `time`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`; types `pthread_t`, `pthread_mutex_t`.
#include <pthread.h>
// Include C library for function `sleep`.
#include <unistd.h>

///
/// Macro
///
#define PHILOSOPHERS_COUNT 5

///
/// Global variables
///
pthread_mutex_t g_kChopsticks[ PHILOSOPHERS_COUNT ];
pthread_t g_kPhilosophers[ PHILOSOPHERS_COUNT ];

///
/// Functions' declarations
///
void* PerformPtr( void* pArgument);
void Think( const int nPhilosopherIndex );
void PickUpChopstick( const int nPhilosopherIndex );
void Eat( const int nPhilosopherIndex );
void PutDownChopstick( const int nPhilosopherIndex );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
	srand( time( NULL ) );
	int nIndex;
	for ( nIndex = 0; nIndex < PHILOSOPHERS_COUNT; nIndex++ )
	{
		pthread_mutex_init( &g_kChopsticks[ nIndex ], NULL );
	}
	for ( nIndex = 0; nIndex < PHILOSOPHERS_COUNT; nIndex++ )
	{
		pthread_create( &g_kPhilosophers[ nIndex ], NULL, PerformPtr, &nIndex );
	}
	for ( nIndex = 0; nIndex < PHILOSOPHERS_COUNT; nIndex++ )
	{
		pthread_join( g_kPhilosophers[ nIndex ], NULL );
	}
	for ( nIndex = 0; nIndex < PHILOSOPHERS_COUNT; nIndex++ )
	{
		pthread_mutex_destroy( &g_kChopsticks[ nIndex ] );
	}
	return 0;
}

///
/// Functions' definitions
///
void* PerformPtr( void* pArgument)
{
	const int nPhilosopherIndex = *( ( int* )pArgument );
	while ( 1 )
	{
		Think( nPhilosopherIndex );
		PickUpChopstick( nPhilosopherIndex );
		Eat( nPhilosopherIndex );
		PutDownChopstick( nPhilosopherIndex );
	}
}

void Think( const int nPhilosopherIndex )
{
	int nThinkingTime = rand() % 3 + 1;
	printf( "Philosopher %d will think for %d second(s).\n", nPhilosopherIndex, nThinkingTime );
	sleep( nThinkingTime );
}

void PickUpChopstick( const int nPhilosopherIndex )
{
	int nRightPhilosopherIndex = ( nPhilosopherIndex + 1 ) % PHILOSOPHERS_COUNT;
	int nLeftPhilosopherIndex  = ( nPhilosopherIndex + PHILOSOPHERS_COUNT ) % PHILOSOPHERS_COUNT;
	if ( nPhilosopherIndex & 1 )
	{
		printf( "Philosopher %d is waiting to pick up chopstick %d.\n", nPhilosopherIndex, nRightPhilosopherIndex );
		pthread_mutex_lock( &g_kChopsticks[ nRightPhilosopherIndex ] );
		printf( "Philosopher %d picked up chopstick %d.\n", nPhilosopherIndex, nRightPhilosopherIndex );
		printf( "Philosopher %d is waiting to pick up chopstick %d.\n", nPhilosopherIndex, nLeftPhilosopherIndex );
		pthread_mutex_lock( &g_kChopsticks[ nLeftPhilosopherIndex ] );
		printf( "Philosopher %d picked up chopstick %d.\n", nPhilosopherIndex, nLeftPhilosopherIndex );
	}
	else
	{
		printf( "Philosopher %d is waiting to pick up chopstick %d.\n", nPhilosopherIndex, nLeftPhilosopherIndex );
		pthread_mutex_lock( &g_kChopsticks[ nLeftPhilosopherIndex ]);
		printf( "Philosopher %d picked up chopstick %d.\n", nPhilosopherIndex, nLeftPhilosopherIndex );
		printf( "Philosopher %d is waiting to pick up chopstick %d.\n", nPhilosopherIndex, nRightPhilosopherIndex );
		pthread_mutex_lock( &g_kChopsticks[ nRightPhilosopherIndex ] );
		printf( "Philosopher %d picked up chopstick %d.\n", nPhilosopherIndex, nRightPhilosopherIndex );
	}
}

void Eat( const int nPhilosopherIndex )
{
	int nEatingTime = rand() % 3 + 1;
	printf( "Philosopher %d will eat for %d second(s).\n", nPhilosopherIndex, nEatingTime );
	sleep( nEatingTime );
}

void PutDownChopstick( const int nPhilosopherIndex )
{
	printf( "Philosopher %d will put down chopsticks.\n", nPhilosopherIndex );
	pthread_mutex_unlock( &g_kChopsticks[ ( nPhilosopherIndex + 1 ) % PHILOSOPHERS_COUNT] );
	pthread_mutex_unlock( &g_kChopsticks[ ( nPhilosopherIndex + PHILOSOPHERS_COUNT ) % PHILOSOPHERS_COUNT ] );
}