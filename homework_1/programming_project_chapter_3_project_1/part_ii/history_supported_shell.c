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
#define MAXIMUM_EXECUTED_COMMANDS_COUNT 1000

///
/// Constants
///
const char AMPERSAND        = '&';
const char END_OF_STRING    = '\0';
const char EXCLAMATION_MARK = '!';
const char* const SPACE = " ";

const char* const COMMAND_EXIT = "exit";
const char* const COMMAND_HISTORY = "history";
const int HISTORY_COMMAND_EXECUTED_COMMANDS_COUNT = 10;
const int    MAXIMUM_COMMAND_ARGUMENTS_COUNT = 40;
const size_t MAXIMUM_COMMAND_ARGUMENT_LENGTH = 10;

const char* const MESSAGE_EXECUTED_COMMANDS_INDEX_IS_INVALID = "No such command in history.";
const char* const MESSAGE_EXECUTED_COMMANDS_IS_EMPTY         = "No commands in history.";
const char* const MESSAGE_REEXECUTE_COMMAND                  = "Re-execute command:";

const int POSIX_ERROR_CODE = -1;

const char* const ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL = "The given array of strings does not end with NULL";
const char* const ERROR_ARRAY_OF_STRINGS_IS_EMPTY = "The given array of strings is empty";
const char* const ERROR_COMMAND_CANNOT_BE_EXECUTED = "The command cannot be executed";
const char* const ERROR_FORK_FAILS = "Fork fails";
const char* const ERROR_NULL_POINTER_INDEX_IS_NEGATIVE = "The given null pointer index is negative.";
const char* const ERROR_STANDARD_INPUT_CANNOT_BE_READ = "The standard input cannot be read";

const int CODE_ERROR_FREE = 0;
const int CODE_ERROR_ARRAY_OF_STRINGS_DOES_NOT_END_WITH_NULL = 1;
const int CODE_ERROR_ARRAY_OF_STRINGS_IS_EMPTY = 2;
const int CODE_ERROR_COMMAND_CANNOT_BE_EXECUTED = 3;
const int CODE_ERROR_FORK_FAILS = 4;
const int CODE_ERROR_NULL_POINTER_INDEX_IS_NEGATIVE = 5;
const int CODE_ERROR_STANDARD_INPUT_CANNOT_BE_READ = 6;

///
/// Global variables
///
char g_kExecutedCommands[ MAXIMUM_EXECUTED_COMMANDS_COUNT ][ MAXIMUM_COMMAND_LENGTH ];
int g_nExecutedCommandsCount;

///
/// Functions' declarations
///
void ExecuteShell( bool* pIsShellRunning );
void InputCommand( char* const pCommand );
void ParseAndExecuteCommand( const char* const pCommand );
void SetCommandArguments( char* * const pCommandArguments, const char* const pCommand );
void SplitStringIntoTokens( char* const pString, const char* const pDelimiter, char* * const pTokens, size_t kTokenLength );
void InsertToken( const char* const pString, char* * const pTokens, const int nTokenIndex, size_t kTokenLength );
bool IsParentProcessWaitingForChildProcess( char* * const pCommandArguments );
int GetLastIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength );
int GetNullPointerIndex( char* * const pArrayOfStrings, const int nArrayMaximumLength );
void RemoveAmpersandArgumentFromCommandArguments( char* * const pCommandArguments );
void CreateChildProcessAndExecuteCommandWithIt( char* * const pCommandArguments, const bool bIsParentProcessWaitingForChildProcess, const char* const pCommand );
void ExecuteCommand( char* * const pCommandArguments, const char* const pCommand );
void ExecuteHistoryCommand();
void ExecuteExclamationMarkCommand( const char* const pExclamationMarkCommand );
void ReexecuteMostRecentExecutedCommand();
void ReexecuteExecutedCommandAt( const int nIndex );
bool IsValidExecutedCommandsIndex( const int nIndex );
void ReexecuteExecutedCommand( const char* const pExecutedCommand );
void ExecuteSystemCommand( char* * const pCommandArguments );
void InsertCommandToExecutedCommands( const char* const pCommand );
void FreeCommandArgumentsDynamicallyAllocatedMemoriesInHeap( char* * const pCommandArguments );

///
/// Main function
///
int main( int nArgumentsCount, char* kArguments[] )
{
    g_nExecutedCommandsCount = 0;
    bool bIsShellRunning = true;
    while ( bIsShellRunning )
    {
        ExecuteShell( &bIsShellRunning );
    }
    return 0;
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
    ParseAndExecuteCommand( kCommand );
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

/* Parse and execute the command. */
void ParseAndExecuteCommand( const char* const pCommand )
{
    char* kCommandArguments[ MAXIMUM_COMMAND_ARGUMENTS_COUNT ];
    SetCommandArguments( kCommandArguments, pCommand );
    bool bIsParentProcessWaitingForChildProcess = IsParentProcessWaitingForChildProcess( kCommandArguments );
    if ( !bIsParentProcessWaitingForChildProcess )
    {
        RemoveAmpersandArgumentFromCommandArguments( kCommandArguments );
    }
    // Although the function `CreateChildProcessAndExecuteCommandWithIt` might modify `kCommandArguments`, the developer did not clone the variable and pass the cloned version as the function argument because this was a tedious task. Instead, the developer chose to document the problem and neglect it to save time.
    CreateChildProcessAndExecuteCommandWithIt( kCommandArguments, bIsParentProcessWaitingForChildProcess, pCommand );
    FreeCommandArgumentsDynamicallyAllocatedMemoriesInHeap( kCommandArguments );
}

/* Set the command arguments according to the given command. */
void SetCommandArguments( char* * const pCommandArguments, const char* const pCommand )
{
    char kClonedCommand[ MAXIMUM_COMMAND_LENGTH ];
    strcpy( kClonedCommand, pCommand );
    SplitStringIntoTokens( kClonedCommand, SPACE, pCommandArguments, MAXIMUM_COMMAND_ARGUMENT_LENGTH );
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
    - This function does not modify `pCommandArguments`. The developer did not add the `const` modifiers because the compiler would yield warnings were he to add. */
bool IsParentProcessWaitingForChildProcess( char* * const pCommandArguments )
{
    return *( *( pCommandArguments + GetLastIndex( pCommandArguments, MAXIMUM_COMMAND_ARGUMENTS_COUNT ) ) ) != AMPERSAND;
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

/* Set the last entry of the command arguments to null. */
void RemoveAmpersandArgumentFromCommandArguments( char* * const pCommandArguments )
{
    *( pCommandArguments + GetLastIndex( pCommandArguments, MAXIMUM_COMMAND_ARGUMENTS_COUNT ) ) = NULL;
}

/*  Create a child process and use it to execute the given command.
    - Creating a child process to execute the system command is necessary as `execvp` will overwrite the current program.
    - This function might modify `pCommandArguments`. The developer was unsure because the argument might be passed into the POSIX API `execvp`. */
void CreateChildProcessAndExecuteCommandWithIt( char* * const pCommandArguments, const bool bIsParentProcessWaitingForChildProcess, const char* const pCommand )
{
    pid_t kProcessId = fork();
    if ( kProcessId == 0 )
    {
        ExecuteCommand( pCommandArguments, pCommand );
        exit( CODE_ERROR_FREE );
    }
    else if ( kProcessId > 0 )
    {
        if ( bIsParentProcessWaitingForChildProcess )
        {
            wait( NULL );
        }
        if ( !( strcmp( pCommand, COMMAND_HISTORY ) == 0 || *( pCommand ) == EXCLAMATION_MARK ) )
        {
            // The developer assumed that the given command must be a system command in this case.
            InsertCommandToExecutedCommands( pCommand );
        }
    }
    else
    {
        perror( ERROR_FORK_FAILS );
        exit( CODE_ERROR_FORK_FAILS );
    }
}

/* Execute the command. */
void ExecuteCommand( char* * const pCommandArguments, const char* const pCommand )
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
        ExecuteSystemCommand( pCommandArguments );
    }
}

/* Execute the history command. */
void ExecuteHistoryCommand()
{
    int nIndex = g_nExecutedCommandsCount;
    while ( true )
    {
        if ( g_nExecutedCommandsCount - nIndex + 1 > HISTORY_COMMAND_EXECUTED_COMMANDS_COUNT )
        {
            break;
        }
        if ( nIndex < 1 )
        {
            break;
        }
        printf( "%d %s\n", nIndex, g_kExecutedCommands[ nIndex ] );
        nIndex--;
    }
}

/* Execute the exclamation mark command. */
void ExecuteExclamationMarkCommand( const char* const pExclamationMarkCommand )
{
    if ( *( pExclamationMarkCommand ) == EXCLAMATION_MARK )
    {
        ReexecuteMostRecentExecutedCommand();
    }
    else
    {
        ReexecuteExecutedCommandAt( atoi( pExclamationMarkCommand ) );
    }
}

/* Re-execute the most recent executed command. */
void ReexecuteMostRecentExecutedCommand()
{
    if ( g_nExecutedCommandsCount == 0 )
    {
        printf( "%s\n", MESSAGE_EXECUTED_COMMANDS_IS_EMPTY );
        return;
    }
    ReexecuteExecutedCommand( g_kExecutedCommands[ g_nExecutedCommandsCount ] );
}

/* Re-execute the executed command at the given index. */
void ReexecuteExecutedCommandAt( const int nIndex )
{
    if ( !IsValidExecutedCommandsIndex( nIndex ) )
    {
        printf( "%s\n", MESSAGE_EXECUTED_COMMANDS_INDEX_IS_INVALID );
        return;
    }
    ReexecuteExecutedCommand( g_kExecutedCommands[ nIndex ] );
}

/* Determine whether the given index is a valid executed commands' index. */
bool IsValidExecutedCommandsIndex( const int nIndex )
{
    return 1 <= nIndex && nIndex <= g_nExecutedCommandsCount;
}

/* Re-execute the given executed command. */
void ReexecuteExecutedCommand( const char* const pExecutedCommand )
{
    printf( "%s %s\n", MESSAGE_REEXECUTE_COMMAND, pExecutedCommand );
    ParseAndExecuteCommand( pExecutedCommand );
}

/*  Execute the given system command.
    - This function might modify `pSystemCommandArguments`. The developer was unsure because the argument would be passed into the POSIX API `execvp`. */
void ExecuteSystemCommand( char* * const pSystemCommandArguments )
{
    int nExecuteFileReturnValue = execvp( *( pSystemCommandArguments ), pSystemCommandArguments );
    if ( nExecuteFileReturnValue == POSIX_ERROR_CODE )
    {
        perror( ERROR_COMMAND_CANNOT_BE_EXECUTED );
        exit( CODE_ERROR_COMMAND_CANNOT_BE_EXECUTED );
    }
}

/* Insert the given command to the executed commands. */
void InsertCommandToExecutedCommands( const char* const pCommand )
{
    g_nExecutedCommandsCount++;
    strcpy( g_kExecutedCommands[ g_nExecutedCommandsCount ], pCommand );
}

/* Free the memories in heap that were dynamically allocated for the command arguments. */
void FreeCommandArgumentsDynamicallyAllocatedMemoriesInHeap( char* * const pCommandArguments )
{
    int nIndex;
    for ( nIndex = 0; nIndex < MAXIMUM_COMMAND_ARGUMENTS_COUNT; nIndex++ )
    {
        if ( *( pCommandArguments + nIndex ) == NULL )
        {
            break;
        }
        free( *( pCommandArguments + nIndex ) );
    }
}