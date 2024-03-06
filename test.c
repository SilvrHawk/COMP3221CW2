Skip to content
 
Search…
All gists
Back to GitHub
@rmcgibbo
rmcgibbo/MPI_ManualReduce.cpp
Last active 2 years ago • Report abuse
Code
Revisions
4
Stars
6
Forks
1
Clone this repository at &lt;script src=&quot;https://gist.github.com/rmcgibbo/7178576.js&quot;&gt;&lt;/script&gt;
<script src="https://gist.github.com/rmcgibbo/7178576.js"></script>
Efficient MPI Parallel Reduction Without MPI_Reduce or MPI_Scan (only Send/Recv)
MPI_ManualReduce.cpp
/*
 * An efficient MPI parallel reduction without MPI_Scan or MPI_Reduce. (i.e. 
 * only send/recv). 
 *
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
 *   0. You just DO WHAT THE FUCK YOU WANT TO.
 */ 
#include <mpi.h>
#include <cstdio>
#include <vector>
static int fastlog2(uint32_t v);


int MPI_ManualReduce(int value) {
  /* 
   * Reduces values on all processes to a single value on rank 0.
   *
   * This function does the same thing as the function MPI_Reduce using
   * only MPI_Send and MPI_Recv. As shown, it operates with additions on
   * integers, so you could trivially use MPI_Reduce, but for operations
   * on variable size structs for which you cannot define an MPI_Datatype,
   * you can still use this method, by modifying it to use your op
   * and your datastructure.
   */ 
  int recvbuffer;
  int tag = 0;
  const int size = MPI::COMM_WORLD.Get_size();
  const int rank = MPI::COMM_WORLD.Get_rank();
  const int lastpower = 1 << fastlog2(size); // No. of procs = 8, lastpower = 3

  // each of the ranks greater than the last power of 2 less than size
  // need to downshift their data, since the binary tree reduction below
  // only works when N is a power of two.
  for (int i = lastpower; i < size; i++)
    if (rank == i)
      MPI::COMM_WORLD.Send(&value, 1, MPI_INT, i-lastpower, tag);
  for (int i = 0; i < size-lastpower; i++)
    if (rank == i) {
      MPI::COMM_WORLD.Recv(&recvbuffer, 1, MPI_INT, i+lastpower, tag);
      value += recvbuffer; // your operation
    }

  for (int d = 0; d < fastlog2(lastpower); d++)
    for (int k = 0; k < lastpower; k += 1 << (d + 1)) {
      const int receiver = k;
      const int sender = k + (1 << d);
      if (rank == receiver) {
        MPI::COMM_WORLD.Recv(&recvbuffer, 1, MPI_INT, sender, tag);
        value += recvbuffer; // your operation
      }
      else if (rank == sender)
        MPI::COMM_WORLD.Send(&value, 1, MPI_INT, receiver, tag);
    }
  return value;
}

static int fastlog2(uint32_t v) {
  // http://graphics.stanford.edu/~seander/bithacks.html
  int r;
  static const int MultiplyDeBruijnBitPosition[32] = 
  {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };

  v |= v >> 1; // first round down to one less than a power of 2 
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  r = MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
  return r;
}



int main(int argc, char* argv[]) {
    MPI::Init(argc, argv);
    const int size = MPI::COMM_WORLD.Get_size();
    const int rank = MPI::COMM_WORLD.Get_rank();

    std::vector<int> data(size);
    for (int i = 0; i < size; i++)
      data[i] = i;
    
    // each rank only gets one entry, and
    // they need to sum them by sending messages
    int result = MPI_ManualReduce(data[rank]);
    MPI::COMM_WORLD.Barrier();

    // Compute the correct result
    int sum = 0;
    for (int i = 0; i < size; i++)
      sum += data[i];
          
          
    if (rank == 0) {
      printf("MPI Result     = %d\n", result);
      printf("Correct Result = %d\n", sum);
    }
    MPI::Finalize();
}
@rmcgibbo
Author
rmcgibbo commented on Oct 27, 2013
mpic++ MPI_ManualReduce.cpp && mpirun -np 11 a.out
@SilvrHawk
Comment
 
Leave a comment
 
Footer
© 2024 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact
Manage cookies
Do not share my personal information
