//
// Routines for use in the coursework. These routines MUST be called as provided;
// DO NOT MODIFY THEM OR REPLACE WITH YOUR OWN VERSION.
//

#include <sys/stat.h>
#include <string.h>

//
// Reads in the text file and returns with a pointer to the character array, which will need to 
// be de-allocated at some point. Prints an error message and returns NULL if this was not possible.
// Adds spaces to the end so that the total length is a multiple of paddingMultiple.
//
char* readText( char *fname, int *size, int paddingMultiple )
{
	// Try to get the file size using stat(), from sys/stat.h.
	struct stat fileStatus;
	if( stat(fname,&fileStatus) )		// stat() returns 0 if there was no error.
	{
		printf( "Could not determine size of the file '%s'.\n", fname );
		return NULL;
	}

	int fileSize = (int) fileStatus.st_size;
	if( fileSize<=0 )
	{
		printf( "File '%s' has bad size (zero or negative).\n", fname );
		return NULL;
	}

	// Round up to include the padding at the end. Keep file size for further checks later on.
	*size = paddingMultiple * ( (fileSize+paddingMultiple-1)/paddingMultiple );

	// Try to open the file.
	FILE *file = fopen( fname, "r" );
	if( !file )
	{
		printf( "Could not open the file '%s' file for reading.\n", fname );
		return NULL;
	}

	// Try to allocate memory for the file.
	char *text = (char*) malloc( *size * sizeof(char) );
	if( text==NULL )
	{
		printf( "Could not allocate memory for the characeter array.\n" );
		fclose(file);
		return NULL;
	}

	// Copy from file, character by character.
	char c;
	int i = 0;
	while( (c=fgetc(file)) != EOF )
	{
		if( i==fileSize )
		{
			printf( "Error in reading text: Reached end of file prematurely.\n" );
			fclose(file);
			return NULL;
		}

		text[i++] = (char) c;
	}

	// Close the file.
	fclose(file);	

	// Check we are where  we expect to be.
	if( i != fileSize )
	{
		printf( "Error reading in the file: Does not match file size.\n" );
		return NULL;
	}

	// Add any padding spaces.
	while( i<*size ) text[i++] = ' ';

	// Return with the pointer to the character array.
	return text;	
}

// Returns the letter code for the given character, case insensitive:
// 'a' or 'A" returns 0
// 'b' or 'B' returns 1
// ...
// 'z' or 'Z' returns 25
// Any non-alphabetic character returns -1.
int letterCodeForChar( char c )
{
	if( c>='a' && c<='z' ) return c-'a';
	if( c>='A' && c<='Z' ) return c-'A';
	
	return -1;
}


// Saves a histogram to file.
void saveHist( int *hist, int size )
{
	FILE *file = fopen( "hist.out", "w" );
	if( !file )
	{
		printf( "Could not open the file for output.\n" );
		return;
	}
	
	int i;
	for( i=0; i<size; i++ ) fprintf( file, "%d %d\n", i, hist[i] );
	
	fclose( file );
}

