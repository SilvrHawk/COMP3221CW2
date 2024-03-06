Complete the table below with your results, and then provide your interpretation at the end.

For the table, only use numeric values, and do not add any comments or additional columns,
as the times and speed-ups will be checked by a script that assumes the exact number of columns
as in the table given below.

Note that:

- When calculating the parallel speed-up S, use the time output by the code, which corresponds
  to the parallel calculation and does not include initialising the character array or
  performing the serial check.

- Take as the serial execution time the time output by the code when run with a single process
  (hence the speed-up for 1 process should be 1.0).


No. Process:          Mean time (average of 3 runs)           Parallel speed-up, S:
============          =============================           =====================
1	                    0.01160                                 1.0
2 	                  0.00702                                 1.652
3 	                  0.00575                                 2.017
4	                    0.00346                                 3.353

Architecture that the timing runs were performed on: desktop or laptop; make; and number of CPU cores:
Desktop. Code is being run on an AMD Ryzen 5 3600 6-core processor.

A brief interpretation of these results (2-3 sentences should be sufficient):
As expected, the mean time reduced as more processes were used in the program. However, likely due to overheads
from MPI, the speed-up wasn't a linear increase and because of this we didn't get a 4x speedup despite using 4x
the processes. One note of interest is that for 3 processes, likely because the binary-tree distribution wasn't used, 
the parallel speed-up rate is very similar to 2 processes, suggesting that the binary-tree does indeed speed processes up,
and looks as if larger number of processes (8, 16, etc) would have larger benefits to the algorithm.

