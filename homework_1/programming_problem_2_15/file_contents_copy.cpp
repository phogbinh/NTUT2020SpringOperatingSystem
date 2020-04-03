// Include C library for flags `O_RDONLY`, `O_WRONLY`, `O_CREAT`.
#include <fcntl.h>
// Include C library for functions `read`, `write`, `open`, `close`.
#include <unistd.h>
// Include C++ standard libraries.
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

const string INSTRUCTION_INPUT_DESTINATION_FILE_NAME = "Input destination file name to copy contents to:";
const string INSTRUCTION_INPUT_SOURCE_FILE_NAME = "Input source file name to copy contents from:";

const string ERROR_CONTENTS_ARE_NOT_COPIED_PROPERLY = "Contents are not copied properly";
const string ERROR_DESTINATION_FILE_CANNOT_BE_CLOSED = "The destination file cannot be closed";
const string ERROR_DESTINATION_FILE_CANNOT_BE_OPENED = "The destination file cannot be opened";
const string ERROR_DESTINATION_FILE_CANNOT_BE_WRITTEN = "The destination file cannot be written";
const string ERROR_SOURCE_FILE_CANNOT_BE_CLOSED = "The source file cannot be closed";
const string ERROR_SOURCE_FILE_CANNOT_BE_OPENED = "The source file cannot be opened";
const string ERROR_SOURCE_FILE_CANNOT_BE_READ = "The source file cannot be read";

const int FILE_NAME_MAXIMUM_CHARACTERS_COUNT = 256; // 2^8
const int POSIX_ERROR_CODE = -1;
const int END_OF_FILE_READ_BYTES_COUNT = 0;

/* Input file name on prompt. */
void InputFileName( const string strInstruction, char ( &kFileName )[ FILE_NAME_MAXIMUM_CHARACTERS_COUNT ] )
{
    string strFileName;
    cout << strInstruction << endl;
    cin >> strFileName;
    strcpy( kFileName, strFileName.c_str() );
}

/* Input the source file name on prompt and open it if exists. */
void OpenSourceFile( int &nSourceFileDescriptor )
{
    char kSourceFileName[ FILE_NAME_MAXIMUM_CHARACTERS_COUNT ];
    InputFileName( INSTRUCTION_INPUT_SOURCE_FILE_NAME, kSourceFileName );
    nSourceFileDescriptor = open( kSourceFileName, O_RDONLY );
    if ( nSourceFileDescriptor == POSIX_ERROR_CODE )
    {
        throw ERROR_SOURCE_FILE_CANNOT_BE_OPENED;
    }
}

/* Input the destination file name on prompt and open it if exists; otherwise, create the destination file. */
void OpenDestinationFile( int &nDestinationFileDescriptor )
{
    const mode_t DESTINATION_FILE_PERMISSION_OCTAL_VALUE = 0644; // - rw- r-- r--
    char kDestinationFileName[ FILE_NAME_MAXIMUM_CHARACTERS_COUNT ];
    InputFileName( INSTRUCTION_INPUT_DESTINATION_FILE_NAME, kDestinationFileName );
    nDestinationFileDescriptor = open( kDestinationFileName, O_WRONLY | O_CREAT, DESTINATION_FILE_PERMISSION_OCTAL_VALUE );
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
    int nSourceFileDescriptor;
    int nDestinationFileDescriptor;
    OpenSourceFile( nSourceFileDescriptor );
    OpenDestinationFile( nDestinationFileDescriptor );
    CopyContentsFromSourceFileToDestinationFile( nSourceFileDescriptor, nDestinationFileDescriptor );
    CloseFile( nSourceFileDescriptor, ERROR_SOURCE_FILE_CANNOT_BE_CLOSED );
    CloseFile( nDestinationFileDescriptor, ERROR_DESTINATION_FILE_CANNOT_BE_CLOSED );
    return 0;
}
