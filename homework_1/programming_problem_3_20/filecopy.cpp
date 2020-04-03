// Include C library for flags `O_RDONLY`, `O_WRONLY`, `O_CREAT`.
#include <fcntl.h>
// Include C library for functions `read`, `write`, `open`, `close`, `fork`, `pipe`.
#include <unistd.h>
// Include C library for function `wait`.
#include <sys/wait.h>
// Include C++ standard libraries.
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

const int END_OF_FILE_READ_BYTES_COUNT = 0;
const int POSIX_ERROR_CODE = -1;

const string ERROR_CONTENTS_ARE_NOT_COPIED_PROPERLY = "Contents are not copied properly";
const string ERROR_DESTINATION_FILE_CANNOT_BE_CLOSED = "The destination file cannot be closed";
const string ERROR_DESTINATION_FILE_CANNOT_BE_OPENED = "The destination file cannot be opened";
const string ERROR_DESTINATION_FILE_CANNOT_BE_WRITTEN = "The destination file cannot be written";
const string ERROR_FORK_FAILS = "Fork fails";
const string ERROR_PIPE_CANNOT_BE_CREATED = "Pipe cannot be created";
const string ERROR_PIPE_READING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED = "The reading file descriptor of pipe cannot be closed";
const string ERROR_PIPE_WRITING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED = "The writing file descriptor of pipe cannot be closed";
const string ERROR_SOURCE_FILE_CANNOT_BE_CLOSED = "The source file cannot be closed";
const string ERROR_SOURCE_FILE_CANNOT_BE_OPENED = "The source file cannot be opened";
const string ERROR_SOURCE_FILE_CANNOT_BE_READ = "The source file cannot be read";

/* Open the source file if it exists. */
void OpenSourceFile( int &nSourceFileDescriptor, const char* const pSourceFileName )
{
    nSourceFileDescriptor = open( pSourceFileName, O_RDONLY );
    if ( nSourceFileDescriptor == POSIX_ERROR_CODE )
    {
        throw ERROR_SOURCE_FILE_CANNOT_BE_OPENED;
    }
}

/* Open the destination file if it exists, and create it otherwise. */
void OpenDestinationFile( int &nDestinationFileDescriptor, const char* const pDestinationFileName )
{
    const mode_t DESTINATION_FILE_PERMISSION_OCTAL_VALUE = 0644; // - rw- r-- r--
    nDestinationFileDescriptor = open( pDestinationFileName, O_WRONLY | O_CREAT, DESTINATION_FILE_PERMISSION_OCTAL_VALUE );
    if ( nDestinationFileDescriptor == POSIX_ERROR_CODE )
    {
        throw ERROR_DESTINATION_FILE_CANNOT_BE_OPENED;
    }
}

/* Read contents from source file and write them to destination file until read fails. */
void CopyContentsFromSourceFileToDestinationFile( int &nSourceFileDescriptor, int &nDestinationFileDescriptor )
{
    const int BUFFER_SIZE = 32768; // 2^15
    ssize_t kReadBytesCount;
    ssize_t kWrittenBytesCount;
    char kBuffer[ BUFFER_SIZE ];
    while ( true )
    {
        kReadBytesCount = read( nSourceFileDescriptor, &kBuffer, BUFFER_SIZE );
        if ( kReadBytesCount == POSIX_ERROR_CODE )
        {
            throw ERROR_SOURCE_FILE_CANNOT_BE_READ;
        }
        if ( kReadBytesCount == END_OF_FILE_READ_BYTES_COUNT )
        {
            break;
        }
        kWrittenBytesCount = write( nDestinationFileDescriptor, &kBuffer, kReadBytesCount );
        if ( kWrittenBytesCount == POSIX_ERROR_CODE )
        {
            throw ERROR_DESTINATION_FILE_CANNOT_BE_WRITTEN;
        }
        if ( kWrittenBytesCount != kReadBytesCount )
        {
            throw ERROR_CONTENTS_ARE_NOT_COPIED_PROPERLY;
        }
    }
}

/* Close file. */
void CloseFile( int &nFileDescriptor, string strErrorMessage )
{
    int nCloseReturnValue = close( nFileDescriptor );
    if ( nCloseReturnValue == POSIX_ERROR_CODE )
    {
        throw strErrorMessage;
    }
}

int main( int nArgumentsCount, char* kArguments[] )
{
    int kPipeFilesDescriptors[ 2 ];
    int* pPipeReadingFileDescriptor = kPipeFilesDescriptors;
    int* pPipeWritingFileDescriptor = kPipeFilesDescriptors + 1;
    int nCreatePipeReturnValue = pipe( kPipeFilesDescriptors );
    if ( nCreatePipeReturnValue == POSIX_ERROR_CODE )
    {
        throw ERROR_PIPE_CANNOT_BE_CREATED;
    }
    pid_t kProcessId = fork();
    if ( kProcessId == 0 )
    {
        cout << "Child process begins." << endl;
        CloseFile( *pPipeWritingFileDescriptor, ERROR_PIPE_WRITING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED );
        int nDestinationFileDescriptor;
        OpenDestinationFile( nDestinationFileDescriptor, kArguments[ 2 ] );
        CopyContentsFromSourceFileToDestinationFile( *pPipeReadingFileDescriptor, nDestinationFileDescriptor ); // Copy contents from pipe to destination file.
        CloseFile( nDestinationFileDescriptor, ERROR_DESTINATION_FILE_CANNOT_BE_CLOSED );
        CloseFile( *pPipeReadingFileDescriptor, ERROR_PIPE_READING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED );
        cout << "Child process ends." << endl;
    }
    else if ( kProcessId > 0 )
    {
        cout << "Parent process begins." << endl;
        CloseFile( *pPipeReadingFileDescriptor, ERROR_PIPE_READING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED );
        int nSourceFileDescriptor;
        OpenSourceFile( nSourceFileDescriptor, kArguments[ 1 ] );
        CopyContentsFromSourceFileToDestinationFile( nSourceFileDescriptor, *pPipeWritingFileDescriptor ); // Copy contents from source file to pipe.
        CloseFile( nSourceFileDescriptor, ERROR_SOURCE_FILE_CANNOT_BE_CLOSED );
        CloseFile( *pPipeWritingFileDescriptor, ERROR_PIPE_WRITING_FILE_DESCRIPTOR_CANNOT_BE_CLOSED );
        cout << "Parent process begins waiting." << endl;
        wait( NULL );
        cout << "Parent process ends waiting." << endl;
        cout << "Parent process ends." << endl;
    }
    else
    {
        throw ERROR_FORK_FAILS;
    }
    return 0;
}
