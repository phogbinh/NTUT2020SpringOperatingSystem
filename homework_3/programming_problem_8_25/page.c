// Include C library for function `atoi`.
#include <stdlib.h>
// Include C library for function `printf`.
#include <stdio.h>

int main( int nArgumentsCount, char* kArguments[] )
{
	unsigned int nVirtualAddress = atoi( kArguments[ 1 ] );
	unsigned int nPageNumber = nVirtualAddress >> 12; // This operation is equivalent to having the virtual address divided by 2^12. The operation shifts the virtual address 12 bits to the right to retrieve its remaining higher bits.
	unsigned int nPageOffset = nVirtualAddress & 0xfff; // The operation AND the virtual address with a binary number having only 12 lower bits ON to retrieve the 12 lower bits of the virtual address. This operating is equivalent to having the virtual address modulo 2^12.
	printf( "page number=%u\n", nPageNumber );
	printf( "offset=%u\n", nPageOffset );
	return 0;
}
