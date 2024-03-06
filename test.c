//
// Specimen answer to the worksheet questions, replacing the point-to-point
// communication with collective communication routines [not including MPI_Reduce()].
//
// Compile with:
//
// mpicc -Wall -o work2_sol4 work2_sol4.c
//


//
// Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>


//
// Problem size (the size of the full data set). Will be rounded up to the
// nearest multiple of the number of processes at run time. Use a very large
// number so that the times are measurable, but not so many as to cause a
// memory allocation failure!
//
#define N 9999999


//
// Main
//
int main( int argc, char **argv )
{
	int i, p, total, globalSize=0, localSize;		// Initialise globalSize purely to avoid compiler warnings.

	//
	// Initialisation
	//

	// Initialise MPI and get the rank and no. of processes.
	int rank, numProcs;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &numProcs );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

	// Allocate memory for the full array on rank 0 only.
	int *globalData = NULL;
	if( rank==0 )
	{
		// Round up to the next-highest multiple of numProcs.
		globalSize = numProcs * ( (N+numProcs-1)/numProcs );

		// Try to allocate memory for the global data array.
		globalData = (int*) malloc( globalSize*sizeof(int) );
		if( !globalData )
		{
			printf( "Could not allocate memory for the global data array.\n" );
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		// Fill the array with random numbers in the range 0 to 99 inclusive. Not a very good way
		// of doing this (will generate a non-uniform distribution), but fine for this example.
		srand( time(NULL) );
		for( i=0; i<globalSize; i++ )
			globalData[i] = rand() % 100;
	}

	// Start the timer.
	double startTime = MPI_Wtime();

	//
	// Step 1. All ranks must know the (dynamic) local array size. For this example they could all calculate
	// it independently, but imagine e.g. rank 0 read the data in from a file.
	//

	// All ranks (including rank 0) have a local array. However, they do not yet know the size, so cannot
	// allocate memory for the array, nor call MPI receive routines with the correct expected message size.

	// Collective version: Use MPI_Bcast();
	if( rank==0 ) localSize = globalSize / numProcs;		// Rank 0 defines the localSize
	MPI_Bcast( &localSize, 1, MPI_INT, 0, MPI_COMM_WORLD );

	// All ranks can now allocate memory for their local arrays.
	int *localData = (int*) malloc( localSize*sizeof(int) );
	if( !localData )
	{
		printf( "Could not allocate memory for the local data array on rank %d.\n", rank );
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	//
	// Step 2. Distribute the global array to the local arrays on all processes.
	//

	// Collective version: Use MPI_Scatter.
	MPI_Scatter(
		globalData, localSize, MPI_INT,						// Data being sent.
		localData , localSize, MPI_INT,						// Data being received.
		0, MPI_COMM_WORLD									// Source rank and communicator - no tag!
	);

	//
	// Step 3. Each process performs the count on its local data. This is purely computation and
	// doesn't need to be modified.
	//
	int count = 0;
	for( i=0; i<localSize; i++ )
		if( localData[i] < 10 ) count++;

	//
	// Step 4. Send all of the local counts back to rank 0, which calculates the total.
	//

	// Collective version: Use MPI_Gather.
	// Needs a bit more work: Cannot keep a running total any more, so create
	// an array of size numProcs in the call to MPI_Gather, then add them up.
	int *partials = NULL;
	if( rank==0 ) partials = malloc( numProcs*sizeof(int) );

	MPI_Gather(
		&count  , 1, MPI_INT,					// Data being sent.
		partials, 1, MPI_INT,					// Data being received.
		0, MPI_COMM_WORLD						// Destination rank and communicator - again, no tag.
	);

	// Now add them up, on rank 0 only. Also free up the (small) memory used for the array of partial counts.
	if( rank==0 )
	{
		total = 0;
		for( p=0; p<numProcs; p++ ) total += partials[p];
		free( partials );
	}

	//
	// Check: Rank 0 performs an independent count on the global data. Rank 0 also outputs
	// how long it took (including allocation of the local arrays).
	//
	if( rank==0 )
	{
		printf( "Time taken: %g s.\n", MPI_Wtime() - startTime );

		int check = 0;
		for( i=0; i<globalSize; i++ )
			if( globalData[i] < 10 ) check++;

		printf( "Distributed count %d (cf. serial count %d).\n", total, check );
	}


	//
	// Clear up and quit.
	//
	if( rank==0 ) free( globalData );
	free( localData );
	MPI_Finalize();
	return EXIT_SUCCESS;
}
