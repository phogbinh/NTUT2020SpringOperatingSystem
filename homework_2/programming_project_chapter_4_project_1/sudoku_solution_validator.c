// Include C library for function `printf`.
#include <stdio.h>
// Include C library for functions `malloc`, `free`.
#include <stdlib.h>
// Include C library for functions `pthread_create`, `pthread_join`; type `pthread_t`.
#include <pthread.h>

///
/// Types
///
typedef enum
{
    false = 0,
    true
} bool;

typedef struct
{
    int nStartingRow;
    int nStartingColumn;
    const int* pSudokuPuzzle;
    int nThreadIndex;
} parameters;

///
/// Macros
///
#define THREADS_COUNT 27
#define SUDOKU_PUZZLE_LINE_ENTRIES_COUNT 9
#define BOX_LINE_ENTRIES_COUNT 3
#define DIGITS_COUNT 10

///
/// Global variable
///
bool g_kIsValidRegion[ THREADS_COUNT ];

///
/// Functions' declarations
///
void CreateThreadsToValidateSudokuPuzzleAndWaitForThem( const int* const pSudokuPuzzle );
void CreateThreadsToValidateSudokuPuzzle( pthread_t* const pThreadsIds, const int* const pSudokuPuzzle );
void CreateThreadsToValidateBoxes( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle );
void CreateThreadsToValidateRows( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle );
void CreateThreadsToValidateColumns( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle );
void CreateThreadToValidateRegion( void* ( *ValidateRegionPtr )( void* ), const int nRow, const int nColumn, const int nThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle );
void* ValidateBoxPtr( void* pArgument );
void* ValidateRowPtr( void* pArgument );
void* ValidateColumnPtr( void* pArgument );
void InitializeIsExistingDigit( bool* const pIsExistingDigit );
bool IsExistingAllDigitsExceptZero( const bool* const pIsExistingDigit );
void SetIsExistingDigitForBox( const int nBoxStartingRow, const int nBoxStartingColumn, bool* const pIsExistingDigit, const int* const pSudokuPuzzle );
void SetIsExistingDigitForRow( const int nRow, bool* const pIsExistingDigit, const int* const pSudokuPuzzle );
void SetIsExistingDigitForColumn( const int nColumn, bool* const pIsExistingDigit, const int* const pSudokuPuzzle );
void WaitForThreads( const pthread_t* const pThreadsIds );
void PrintValidatedResult();

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    /* The Sudoku puzzle to check. */
    int kSudokuPuzzle[ SUDOKU_PUZZLE_LINE_ENTRIES_COUNT ][ SUDOKU_PUZZLE_LINE_ENTRIES_COUNT ] =
    {
        {3, 6, 5, 9, 4, 8, 7, 1, 2},
        {4, 8, 2, 7, 1, 3, 5, 9, 6},
        {9, 7, 1, 5, 2, 6, 4, 3, 8},
        {6, 4, 7, 8, 3, 9, 1, 2, 5},
        {5, 2, 3, 6, 7, 1, 8, 4, 9},
        {8, 1, 9, 2, 5, 4, 3, 6, 7},
        {7, 9, 4, 1, 6, 5, 2, 8, 3},
        {2, 3, 8, 4, 9, 7, 6, 5, 1},
        {1, 5, 6, 3, 8, 2, 9, 7, 4}
    };
    CreateThreadsToValidateSudokuPuzzleAndWaitForThem( ( int* )kSudokuPuzzle );
    PrintValidatedResult();
}

///
/// Functions' definitions
///
void CreateThreadsToValidateSudokuPuzzleAndWaitForThem( const int* const pSudokuPuzzle )
{
    pthread_t kThreadsIds[ THREADS_COUNT ];
    CreateThreadsToValidateSudokuPuzzle( kThreadsIds, pSudokuPuzzle );
    WaitForThreads( kThreadsIds );
}

void CreateThreadsToValidateSudokuPuzzle( pthread_t* const pThreadsIds, const int* const pSudokuPuzzle )
{
    int nThreadIndex = 0;
    CreateThreadsToValidateBoxes  ( &nThreadIndex, pThreadsIds, pSudokuPuzzle );
    CreateThreadsToValidateRows   ( &nThreadIndex, pThreadsIds, pSudokuPuzzle );
    CreateThreadsToValidateColumns( &nThreadIndex, pThreadsIds, pSudokuPuzzle );
}

void CreateThreadsToValidateBoxes( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle )
{
    int nRow;
    int nColumn;
    for ( nRow = 0; nRow < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nRow++ )
    {
        for ( nColumn = 0; nColumn < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nColumn++ )
        {
            if ( nRow % BOX_LINE_ENTRIES_COUNT == 0 && nColumn % BOX_LINE_ENTRIES_COUNT == 0 )
            {
                CreateThreadToValidateRegion( ValidateBoxPtr, nRow, nColumn, *pThreadIndex, pThreadsIds, pSudokuPuzzle );
                ( *pThreadIndex )++;
            }
        }
    }
}

void CreateThreadsToValidateRows( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle )
{
    int nRow;
    int nColumn;
    for ( nRow = 0; nRow < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nRow++ )
    {
        for ( nColumn = 0; nColumn < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nColumn++ )
        {
            if ( nColumn == 0 )
            {
                CreateThreadToValidateRegion( ValidateRowPtr, nRow, nColumn, *pThreadIndex, pThreadsIds, pSudokuPuzzle );
                ( *pThreadIndex )++;
            }
        }
    }
}

void CreateThreadsToValidateColumns( int* const pThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle )
{
    int nRow;
    int nColumn;
    for ( nRow = 0; nRow < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nRow++ )
    {
        for ( nColumn = 0; nColumn < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nColumn++ )
        {
            if ( nRow == 0 )
            {
                CreateThreadToValidateRegion( ValidateColumnPtr, nRow, nColumn, *pThreadIndex, pThreadsIds, pSudokuPuzzle );
                ( *pThreadIndex )++;
            }
        }
    }
}

void CreateThreadToValidateRegion( void* ( *ValidateRegionPtr )( void* ), const int nRow, const int nColumn, const int nThreadIndex, pthread_t* const pThreadsIds, const int* const pSudokuPuzzle )
{
    parameters* pParameters = ( parameters* )malloc( sizeof( parameters ) );
    pParameters->nStartingRow    = nRow;
    pParameters->nStartingColumn = nColumn;
    pParameters->pSudokuPuzzle   = pSudokuPuzzle;
    pParameters->nThreadIndex    = nThreadIndex;
    pthread_create( pThreadsIds + nThreadIndex, NULL, ValidateRegionPtr, pParameters );
}

void* ValidateBoxPtr( void* pArgument )
{
    parameters* pParameters = ( parameters* )pArgument;
    bool kIsExistingDigit[ DIGITS_COUNT ];
    InitializeIsExistingDigit( kIsExistingDigit );
    SetIsExistingDigitForBox( pParameters->nStartingRow, pParameters->nStartingColumn, kIsExistingDigit, pParameters->pSudokuPuzzle );
    g_kIsValidRegion[ pParameters->nThreadIndex ] = IsExistingAllDigitsExceptZero( kIsExistingDigit );
    free( pParameters );
}

void* ValidateRowPtr( void* pArgument )
{
    parameters* pParameters = ( parameters* )pArgument;
    bool kIsExistingDigit[ DIGITS_COUNT ];
    InitializeIsExistingDigit( kIsExistingDigit );
    SetIsExistingDigitForRow( pParameters->nStartingRow, kIsExistingDigit, pParameters->pSudokuPuzzle );
    g_kIsValidRegion[ pParameters->nThreadIndex ] = IsExistingAllDigitsExceptZero( kIsExistingDigit );
    free( pParameters );
}

void* ValidateColumnPtr( void* pArgument )
{
    parameters* pParameters = ( parameters* )pArgument;
    bool kIsExistingDigit[ DIGITS_COUNT ];
    InitializeIsExistingDigit( kIsExistingDigit );
    SetIsExistingDigitForColumn( pParameters->nStartingColumn, kIsExistingDigit, pParameters->pSudokuPuzzle );
    g_kIsValidRegion[ pParameters->nThreadIndex ] = IsExistingAllDigitsExceptZero( kIsExistingDigit );
    free( pParameters );
}

void InitializeIsExistingDigit( bool* const pIsExistingDigit )
{
    int nDigit;
    for ( nDigit = 0; nDigit < DIGITS_COUNT; nDigit++ )
    {
        *( pIsExistingDigit + nDigit ) = false;
    }
}

bool IsExistingAllDigitsExceptZero( const bool* const pIsExistingDigit )
{
    int nDigit;
    for ( nDigit = 1; nDigit < DIGITS_COUNT; nDigit++ )
    {
        if ( *( pIsExistingDigit + nDigit ) == false )
        {
            return false;
        }
    }
    return true;
}

void SetIsExistingDigitForBox( const int nBoxStartingRow, const int nBoxStartingColumn, bool* const pIsExistingDigit, const int* const pSudokuPuzzle )
{
    int nRow;
    int nColumn;
    for ( nRow = nBoxStartingRow; nRow < nBoxStartingRow + BOX_LINE_ENTRIES_COUNT; nRow++ )
    {
        for ( nColumn = nBoxStartingColumn; nColumn < nBoxStartingColumn + BOX_LINE_ENTRIES_COUNT; nColumn++ )
        {
            int nEntryIndex = SUDOKU_PUZZLE_LINE_ENTRIES_COUNT * nRow + nColumn;
            int nEntryValue = *( pSudokuPuzzle + nEntryIndex );
            *( pIsExistingDigit + nEntryValue ) = true;
        }
    }
}

void SetIsExistingDigitForRow( const int nRow, bool* const pIsExistingDigit, const int* const pSudokuPuzzle )
{
    int nColumn;
    for ( nColumn = 0; nColumn < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nColumn++ )
    {
        int nEntryIndex = SUDOKU_PUZZLE_LINE_ENTRIES_COUNT * nRow + nColumn;
        int nEntryValue = *( pSudokuPuzzle + nEntryIndex );
        *( pIsExistingDigit + nEntryValue ) = true;
    }
}

void SetIsExistingDigitForColumn( const int nColumn, bool* const pIsExistingDigit, const int* const pSudokuPuzzle )
{
    int nRow;
    for ( nRow = 0; nRow < SUDOKU_PUZZLE_LINE_ENTRIES_COUNT; nRow++ )
    {
        int nEntryIndex = SUDOKU_PUZZLE_LINE_ENTRIES_COUNT * nRow + nColumn;
        int nEntryValue = *( pSudokuPuzzle + nEntryIndex );
        *( pIsExistingDigit + nEntryValue ) = true;
    }
}

void WaitForThreads( const pthread_t* const pThreadsIds )
{
    int nIndex;
    for ( nIndex = 0; nIndex < THREADS_COUNT; nIndex++ )
    {
        pthread_join( *( pThreadsIds + nIndex ), NULL );
    }
}

void PrintValidatedResult()
{
    int nThreadIndex;
    for ( nThreadIndex = 0; nThreadIndex < THREADS_COUNT; nThreadIndex++ )
    {
        if ( !g_kIsValidRegion[ nThreadIndex ] )
        {
            printf( "The given Sudoku puzzle is invalid.\n" );
            return;
        }
    }
    printf( "The given Sudoku puzzle is valid.\n" );
}