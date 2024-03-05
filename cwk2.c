//
// Starting code for the MPI coursework.
//
// Compile with:
//
// mpicc -Wall -o cwk2 cwk2.c
//
// or use the provided makefile.
//


//
// Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


// Some extra routines for this coursework. DO NOT MODIFY OR REPLACE THESE ROUTINES,
// as this file will be replaced with a different version for assessment.
#include "cwk2_extra.h"


//
// Case is not considered (i.e. 'a' is the same as 'A'), and any non-alphabetic characters
// are ignored, so only need to count 26 different possibilities.
//
#define MAX_LETTERS 26


//
// Main
//
int main( int argc, char **argv )
{
	int i, lc;

	// Initialise MPI and get the rank and no. of processes.
	int rank, numProcs;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &numProcs );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

	// Read in the text file to rank 0.
	char *fullText = NULL;
	int totalChars = 0;
	if( rank==0 )
	{
		// Try to read in the file. The pointer 'text' must be free()'d before termination. Will add spaces to the
		// end so that the total size of the text array is a multiple of numProcs. Will print an error message and
		// return NULL if the operation could not be completed for any reason.
		fullText = readText( "input.txt", &totalChars, numProcs );
		if( fullText==NULL )
		{
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		printf( "Rank 0: Read in text file with %d characters (including padding).\n", totalChars );
	}

	// The final global histogram - declared for all processes but the final answer will only be on rank 0.
	int globalHist[MAX_LETTERS];
	for( i=0; i<MAX_LETTERS; i++ ) globalHist[i] = 0;		// Initialise to zero.

	// Calculate the number of characters per process. Note that only rank 0 has the correct value of totalChars
	// (and hence charsPerproc) at this point. Also, we know by this point that totalChars is a multiple of numProcs.
	int charsPerProc = totalChars / numProcs;

	// Start the timing.
	double startTime = MPI_Wtime();


	//
	// Your solution will primarily go here, although dynamic memory allocation and freeing may go elsewhere.
	//


	// Complete the timing and output the result.
	if( rank==0 )
		printf( "Distribution, local calculation and reduction took a total time: %g s\n", MPI_Wtime() - startTime );

	//
	// Check against the serial calculation (rank 0 only).
	//
	if( rank==0 )
	{
		printf( "\nChecking final histogram against the serial calculation.\n" );

		// Initialise the serial check histogram.
		int serialHist[MAX_LETTERS];
		for( i=0; i<MAX_LETTERS; i++ ) serialHist[i] = 0;

		// Construct the serial histogram as per the parallel version, but over the whole text.
		for( i=0; i<totalChars; i++ )
			if( (lc=letterCodeForChar(fullText[i]))!= -1 )
				serialHist[lc]++;

		// Check for errors (i.e. differences to the serial calculation).
		int errorFound = 0;
		for( i=0; i<MAX_LETTERS; i++ )
		{
			if( globalHist[i] != serialHist[i] ) errorFound = 1;
			printf( "For letter code %2i ('%c' or '%c'): MPI count (serial check) = %4i (%4i)\n", i, 'a'+i, 'A'+i, globalHist[i], serialHist[i] );
		}

		if( errorFound )
			printf( "- at least one error found when checking against the serial calculation.\n" );
		else
			printf( " - globalHist has the same values as the serial check.\n" );
	}

	//
	// Clear up and quit.
	//
	if( rank==0 )
	{
		saveHist( globalHist, MAX_LETTERS );			// Defined in cwk2_extras.h; do not change or replace the call.
		free( fullText );
	}
	
	MPI_Finalize();
	return EXIT_SUCCESS;
}