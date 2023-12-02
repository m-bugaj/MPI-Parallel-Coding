# MPI Random Voting Program

## Overview
This MPI program demonstrates a distributed random voting system among N processes (P1...PN). The processes communicate to democratically choose one of them through rounds of voting. Initially, all processes are candidates, and in each round, candidates with zero votes and the candidate with the smallest nonzero number of votes are eliminated. The voting mechanism is encapsulated in the `Vote(t)` subroutine, where each process decides randomly which candidate to vote for based on the given array `t` representing the vote count for each process.

## Configuration
Before running the program, ensure that the MPI project is configured properly in Visual Studio:

- Project -> C/C++ -> General -> Additional Include Directories:
  - $(MSMPI_INC)
  - $(MSMPI_INC)\x64

- Project -> Linker -> All Options -> Additional Library Directories:
  - $(MSMPI_LIB64)

- Project -> Linker -> All Options -> Additional Dependencies:
  - msmpi.lib

Also, download the MS-MPI SDK from [MSDN](https://msdn.microsoft.com/en-us/library/bb524831.aspx) for installation files.

## Execution
Compile the project in Visual Studio and run the executable located in `x64/Debug` directory. Use the following command to execute the program with an odd number of processes:

```bash
mpiexec -n 11 mpiLab.exe
```

Replace '11' with the desired number of processes. The program showcases the MPI random voting algorithm, with each process actively participating in the voting process.

Important Note
Ensure that the number of processes passed to mpiexec -n is odd, as the algorithm requires an odd number of participants for proper functioning.

🚀 Happy Voting! 🗳️
