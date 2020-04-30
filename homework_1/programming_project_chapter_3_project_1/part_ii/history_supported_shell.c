// Include C library for functions `printf`, `fflush`, `perror`.
#include <stdio.h>
// Include C library for functions `exit`, `malloc`, `free`.
#include <stdlib.h>
// Include C library for function `wait`.
#include <sys/wait.h>
// Include C library for functions `fork`, `read`, `execvp`; macro `STDIN_FILENO`.
#include <unistd.h>
// Include C library for functions `strtok`, `strcmp`, `strcpy`, `atoi`.
#include <string.h>

///
/// Type
///
typedef enum
{
    false = 0,
    true
} bool;

///
/// Macros
///
#define MAXIMUM_COMMAND_LENGTH 500
#define MAXIMUM_EXECUTED_SYSTEM_COMMANDS_COUNT 1000

///
/// Constants
///
const char AMPERSAND        = '&';
const char END_OF_STRING    = '\0';
const char EXCLAMATION_MARK = '!';
const char* const SPACE = " ";

const char* const COMMAND_EXIT    = "exit";
const char* const COMMAND_HISTORY = "history";
const int HISTORY_COMMAND_EXECUTED_SYSTEM_COMMANDS_COUNT = 10;
const int    MAXIMUM_SYSTEM_COMMAND_ARGUMENTS_COUNT = 40;
const size_t MAXIMUM_SYSTEM_COMMAND_ARGUMENT_LENGTH = 10;

const char* const MESSAGE_EXECUTED_SYSTEM_COMMANDS_INDEX_IS_OUT_OF_RANGE = "No such command in history.";
const char* const MESSAGE_EXECUTED_SYSTEM_COMMANDS_IS_EMPTY              = "No commands in history.";
const char* const MESSAGE_REEXECUTE_SYSTEM_COMMAND                       = "Re-execute system command:";

const int POSIX_ERROR_CODE = -1;

const char* const ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL = "The given array of strings does not end with NULL";
const char* const ERROR_ARRAY_OF_STRINGS_IS_EMPTY               = "The given array of strings is empty";
const char* const ERROR_SYSTEM_COMMAND_CANNOT_BE_EXECUTED       = "The system command cannot be executed";
const char* const ERROR_FORK_FAILS                              = "Fork fails";
const char* const ERROR_NULL_POINTER_INDEX_IS_NEGATIVE          = "The given null pointer index is negative.";
const char* const ERROR_STANDARD_INPUT_CANNOT_BE_READ           = "The standard input cannot be read";

const int CODE_ERROR_FREE                                    = 0;
const int CODE_ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL = 1;
const int CODE_ERROR_ARRAY_OF_STRINGS_IS_EMPTY               = 2;
const int CODE_ERROR_SYSTEM_COMMAND_CANNOT_BE_EXECUTED       = 3;
const int CODE_ERROR_FORK_FAILS                              = 4;
const int CODE_ERROR_NULL_POINTER_INDEX_IS_NEGATIVE          = 5;
const int CODE_ERROR_STANDARD_INPUT_CANNOT_BE_READ           = 6;

///
/// Global variables
///
char g_kExecutedSystemCommands[ MAXIMUM_EXECUTED_SYSTEM_COMMANDS_COUNT ][ MAXIMUM_COMMAND_LENGTH ];
int g_nExecutedSystemCommandsCount;

///
/// Functions' declarations
///
void ExecuteShell( bool* pIsShellRunning );
void InputCommand( char* const pCommand );
void ExecuteCommand( const char* const pCommand );
void ExecuteHistoryCommand();
void ExecuteExclamationMarkCommand( const char* const pExclamationMarkCommand );
void ReexecuteMostRecentExecutedSystemCommand();
void ReexecuteExecutedSystemCommandAt( const int nIndex );
bool IsInRangeExecutedSystemCommandsIndex( const int nIndex );
void ReexecuteExecutedSystemCommand( const char* const pExecutedSystemCommand );
void InsertSystemCommandToExecutedSystemCommands( const char* const pSystemCommand );
void ParseAndExecuteSystemCommand( const char* const pSystemCommand );
void SetSystemCommandArguments( char* * const pSystemCommandArguments, const char* const pSystemCommand );
void SplitStringIntoTokens( char* const pString, const char* const pDelimiter, char* * const pTokens, size_t kTokenLength );
void InsertToken( const char* const pString, char* * const pTokens, const int nTokenIndex, size_t kTokenLength );
bool IsParentProcessWaitingForChildProcess( char* * const pSystemCommandArguments );
int GetLastIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength );
int GetNullPointerIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength );
void RemoveAmpersandArgumentFromSystemCommandArguments( char* * const pSystemCommandArguments );
void CreateChildProcessAndExecuteSystemCommandWithIt( char* * const pSystemCommandArguments, const bool bIsParentProcessWaitingForChildProcess );
void ExecuteSystemCommand( char* * const pSystemCommandArguments );
void FreeSystemCommandArgumentsDynamicallyAllocatedMemoriesInHeap( char* * const pSystemCommandArguments );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    g_nExecutedSystemCommandsCount = 0;
    bool bIsShellRunning = true;
    while ( bIsShellRunning )
    {
        ExecuteShell( &bIsShellRunning );
    }
    return CODE_ERROR_FREE;
}

///
/// Functions' definitions
///
/* Execute the shell. */
void ExecuteShell( bool* pIsShellRunning )
{
    char kCommand[ MAXIMUM_COMMAND_LENGTH ];
    InputCommand( kCommand );
    if ( strcmp( kCommand, COMMAND_EXIT ) == 0 )
    {
        *pIsShellRunning = false;
        return;
    }
    ExecuteCommand( kCommand );
}

/* Input the command from the standard input. */
void InputCommand( char* const pCommand )
{
    printf( "osh>" );
    fflush( stdout );
    ssize_t kReadBytesCount = read( STDIN_FILENO, pCommand, MAXIMUM_COMMAND_LENGTH );
    if ( kReadBytesCount == POSIX_ERROR_CODE )
    {
        perror( ERROR_STANDARD_INPUT_CANNOT_BE_READ );
        exit( CODE_ERROR_STANDARD_INPUT_CANNOT_BE_READ );
    }
    // Remove the end-of-line character generated by user hitting enter.
    *( pCommand + kReadBytesCount - 1 ) = END_OF_STRING;
}

/* Execute the given command. */
void ExecuteCommand( const char* const pCommand )
{
    if ( strcmp( pCommand, COMMAND_HISTORY ) == 0 )
    {
        ExecuteHistoryCommand();
    }
    else if ( *( pCommand ) == EXCLAMATION_MARK )
    {
        ExecuteExclamationMarkCommand( pCommand + 1 );
    }
    else
    {
        // The developer assumed that the given command must be a system command in this case.
        InsertSystemCommandToExecutedSystemCommands( pCommand );
        ParseAndExecuteSystemCommand( pCommand );
    }
}

/* Execute the history command. */
void ExecuteHistoryCommand()
{
    int nIndex = g_nExecutedSystemCommandsCount;
    while ( true )
    {
        if ( g_nExecutedSystemCommandsCount - nIndex + 1 > HISTORY_COMMAND_EXECUTED_SYSTEM_COMMANDS_COUNT )
        {
            break;
        }
        if ( nIndex < 1 )
        {
            break;
        }
        printf( "%d %s\n", nIndex, g_kExecutedSystemCommands[ nIndex ] );
        nIndex--;
    }
}

/* Execute the exclamation mark command. */
void ExecuteExclamationMarkCommand( const char* const pExclamationMarkCommand )
{
    if ( *( pExclamationMarkCommand ) == EXCLAMATION_MARK )
    {
        ReexecuteMostRecentExecutedSystemCommand();
    }
    else
    {
        ReexecuteExecutedSystemCommandAt( atoi( pExclamationMarkCommand ) );
    }
}

/* Re-execute the most recent executed system command. */
void ReexecuteMostRecentExecutedSystemCommand()
{
    if ( g_nExecutedSystemCommandsCount == 0 )
    {
        printf( "%s\n", MESSAGE_EXECUTED_SYSTEM_COMMANDS_IS_EMPTY );
        return;
    }
    ReexecuteExecutedSystemCommand( g_kExecutedSystemCommands[ g_nExecutedSystemCommandsCount ] );
}

/* Re-execute the executed system command at the given index. */
void ReexecuteExecutedSystemCommandAt( const int nIndex )
{
    if ( !IsInRangeExecutedSystemCommandsIndex( nIndex ) )
    {
        printf( "%s\n", MESSAGE_EXECUTED_SYSTEM_COMMANDS_INDEX_IS_OUT_OF_RANGE );
        return;
    }
    ReexecuteExecutedSystemCommand( g_kExecutedSystemCommands[ nIndex ] );
}

/* Determine whether the given index is an in range executed system commands' index. */
bool IsInRangeExecutedSystemCommandsIndex( const int nIndex )
{
    return 1 <= nIndex && nIndex <= g_nExecutedSystemCommandsCount;
}

/* Re-execute the given executed system command. */
void ReexecuteExecutedSystemCommand( const char* const pExecutedSystemCommand )
{
    printf( "%s %s\n", MESSAGE_REEXECUTE_SYSTEM_COMMAND, pExecutedSystemCommand );
    InsertSystemCommandToExecutedSystemCommands( pExecutedSystemCommand );
    ParseAndExecuteSystemCommand( pExecutedSystemCommand );
}

/* Insert the given system command to the executed system commands. */
void InsertSystemCommandToExecutedSystemCommands( const char* const pSystemCommand )
{
    g_nExecutedSystemCommandsCount++;
    strcpy( g_kExecutedSystemCommands[ g_nExecutedSystemCommandsCount ], pSystemCommand );
}

/* Parse and execute the given system command. */
void ParseAndExecuteSystemCommand( const char* const pSystemCommand )
{
    char* kSystemCommandArguments[ MAXIMUM_SYSTEM_COMMAND_ARGUMENTS_COUNT ];
    SetSystemCommandArguments( kSystemCommandArguments, pSystemCommand );
    bool bIsParentProcessWaitingForChildProcess = IsParentProcessWaitingForChildProcess( kSystemCommandArguments );
    if ( !bIsParentProcessWaitingForChildProcess )
    {
        RemoveAmpersandArgumentFromSystemCommandArguments( kSystemCommandArguments );
    }
    CreateChildProcessAndExecuteSystemCommandWithIt( kSystemCommandArguments, bIsParentProcessWaitingForChildProcess );
    FreeSystemCommandArgumentsDynamicallyAllocatedMemoriesInHeap( kSystemCommandArguments );
}

/* Set the system command arguments according to the given system command. */
void SetSystemCommandArguments( char* * const pSystemCommandArguments, const char* const pSystemCommand )
{
    char kClonedSystemCommand[ MAXIMUM_COMMAND_LENGTH ];
    strcpy( kClonedSystemCommand, pSystemCommand );
    SplitStringIntoTokens( kClonedSystemCommand, SPACE, pSystemCommandArguments, MAXIMUM_SYSTEM_COMMAND_ARGUMENT_LENGTH );
}

/*  Split the given string into tokens by the given delimiter.
    - Each token is a substring of the given string.
    - Memory of each token is allocated dynamically in heap, each accounts for `kTokenLength` bytes. Thus, the tokens' life cycles are independent of the string's.
    - The given string's contents will be modified by `strtok`: all delimiters will be replaced with end-of-string characters. */
void SplitStringIntoTokens( char* const pString, const char* const pDelimiter, char* * const pTokens, size_t kTokenLength )
{
    int nIndexCounter = 0;
    // Insert the first token. 
    char* pPointerToCharacterOnImmediateRightOfFirstFoundDelimiterInString = strtok( pString, pDelimiter );
    InsertToken( pPointerToCharacterOnImmediateRightOfFirstFoundDelimiterInString, pTokens, nIndexCounter, kTokenLength );
    nIndexCounter++;
    // Insert the remaining tokens (if any).
    while ( pPointerToCharacterOnImmediateRightOfFirstFoundDelimiterInString != NULL )
    {
        pPointerToCharacterOnImmediateRightOfFirstFoundDelimiterInString = strtok( NULL, pDelimiter );
        InsertToken( pPointerToCharacterOnImmediateRightOfFirstFoundDelimiterInString, pTokens, nIndexCounter, kTokenLength );
        nIndexCounter++;
    }
}

/*  Insert the given string as a token to the given tokens.
    - If the given string pointer is null, which occurs when `strtok` returns null, then a null token is inserted to the tokens indicating the tokens' end. */
void InsertToken( const char* const pString, char* * const pTokens, const int nTokenIndex, size_t kTokenLength )
{
    if ( pString == NULL )
    {
        *( pTokens + nTokenIndex ) = NULL;
        return;
    }
    *( pTokens + nTokenIndex ) = ( char* )malloc( kTokenLength );
    strcpy( *( pTokens + nTokenIndex ), pString );
}

/*  Determine whether the parent process must waits for the child process finishing execution before continuing its own.
    - This function does not modify `pSystemCommandArguments`. The developer did not add the `const` modifiers because the compiler would yield warnings were he to add. */
bool IsParentProcessWaitingForChildProcess( char* * const pSystemCommandArguments )
{
    return *( *( pSystemCommandArguments + GetLastIndex( pSystemCommandArguments, MAXIMUM_SYSTEM_COMMAND_ARGUMENTS_COUNT ) ) ) != AMPERSAND;
}

/*  Get the last index of an array of strings, whose corresponding string precedes the null pointer.
    - This function does not modify `pArrayOfStrings`. The developer did not add the `const` modifiers because the compiler would yield warnings were he to add. */
int GetLastIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength )
{
    int nNullPointerIndex = GetNullPointerIndex( pArrayOfStrings, nArrayMaximumLength );
    if ( nNullPointerIndex == 0 )
    {
        perror( ERROR_ARRAY_OF_STRINGS_IS_EMPTY );
        exit( CODE_ERROR_ARRAY_OF_STRINGS_IS_EMPTY );
    }
    if ( nNullPointerIndex < 0 )
    {
        perror( ERROR_NULL_POINTER_INDEX_IS_NEGATIVE );
        exit( CODE_ERROR_NULL_POINTER_INDEX_IS_NEGATIVE );
    }
    return nNullPointerIndex - 1;
}

/*  Get the null pointer's index of an array of strings.
    - This function does not modify `pArrayOfStrings`. The developer did not add the `const` modifiers because the compiler would yield warnings were he to add. */
int GetNullPointerIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength )
{
    int nIndex = 0;
    while ( true )
    {
        if ( nIndex == nArrayMaximumLength )
        {
            perror( ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL );
            exit( CODE_ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL );
        }
        char* pString = *( pArrayOfStrings + nIndex );
        if ( pString == NULL )
        {
            return nIndex;
        }
        nIndex++;
    }
}

/* Set the last entry of the system command arguments to null. */
void RemoveAmpersandArgumentFromSystemCommandArguments( char* * const pSystemCommandArguments )
{
    *( pSystemCommandArguments + GetLastIndex( pSystemCommandArguments, MAXIMUM_SYSTEM_COMMAND_ARGUMENTS_COUNT ) ) = NULL;
}

/*  Create a child process and use it to execute the given system command.
    - Creating a child process to execute the system command is necessary as `execvp` will overwrite the current program.
    - This function does not modify `pSystemCommandArguments` in the parent process.
    - This function might modify `pSystemCommandArguments` in the child process. The developer was unsure because the argument would be passed into the POSIX API `execvp`. */
void CreateChildProcessAndExecuteSystemCommandWithIt( char* * const pSystemCommandArguments, const bool bIsParentProcessWaitingForChildProcess )
{
    pid_t kProcessId = fork();
    if ( kProcessId == 0 )
    {
        // `ExecuteSystemCommand` calls `execvp`, replacing the current process image with a new one, effectively overwriting any data of the current process image (including the process's dynamically allocated memories in heap). Thus, no recycling work for the system command arguments needs to be done here.
        ExecuteSystemCommand( pSystemCommandArguments );
    }
    else if ( kProcessId > 0 )
    {
        if ( bIsParentProcessWaitingForChildProcess )
        {
            wait( NULL );
        }
    }
    else
    {
        perror( ERROR_FORK_FAILS );
        exit( CODE_ERROR_FORK_FAILS );
    }
}

/*  Execute the given system command.
    - This function might modify `pSystemCommandArguments`. The developer was unsure because the argument would be passed into the POSIX API `execvp`. */
void ExecuteSystemCommand( char* * const pSystemCommandArguments )
{
    int nExecuteFileReturnValue = execvp( *( pSystemCommandArguments ), pSystemCommandArguments );
    if ( nExecuteFileReturnValue == POSIX_ERROR_CODE )
    {
        perror( ERROR_SYSTEM_COMMAND_CANNOT_BE_EXECUTED );
        exit( CODE_ERROR_SYSTEM_COMMAND_CANNOT_BE_EXECUTED );
    }
}

/* Free the memories in heap that were dynamically allocated for the system command arguments. */
void FreeSystemCommandArgumentsDynamicallyAllocatedMemoriesInHeap( char* * const pSystemCommandArguments )
{
    int nIndex;
    for ( nIndex = 0; nIndex < MAXIMUM_SYSTEM_COMMAND_ARGUMENTS_COUNT; nIndex++ )
    {
        if ( *( pSystemCommandArguments + nIndex ) == NULL )
        {
            break;
        }
        free( *( pSystemCommandArguments + nIndex ) );
    }
}
