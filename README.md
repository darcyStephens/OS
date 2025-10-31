## Assignment 3: Concurrency

- Authors: Darcy Stephens, Shreeshail Bhatta, Jessica Hu
- Group name: Group 199

## Overview

This program is the implementation of the mergesort algorithm with parallel threading incorporated to add deeper layers. In essence, it sorts a given array by first recursively building new threads to sort new (and smaller) sub-arrays with each level of recursion, until it happens to meet a hard cutoff which is defined by user or hardware limitations.

## Manifest

A listing of source files and other non-generated files, and a brief
(one-line) explanation of the purpose of each file.

- mergesort.c  
  This is the main file holding the mergesort algorithm implementation source code.

- mergesort.h  
  This file declares and defines the functions, global variables, and data structures that are to be used in the mergesort.c file.

- mergetester.c  
  This file contains ???

- test-mergesort.c  
  This file tests whether the mergesort implementation works as expected based on validation of sorting a randomly generated array and timing.

* tester.sh  
  This file contains a bash script that compiles mergesort.c and runs testing with a list of parameters to test the correctness of the program.

* plot_performance.py  
  This file visualises the performance of the margesort program based on timing.

* Makefile  
  This file compiles the mergesort.c and test-mergesort.c to be executed.

* README.md  
  This file provides an overview for the assignment and program, specifying features, providing a guide for using the program, and a reflection of the project development process.

## Building the project

This section should tell the user how to build your code. If you are
delivering a library, where does it need to be installed, or how do you use
it? Is this an executable, if so, how can a user get up to speed as fast as
possible?

#### Running the main program

To run the main program, build with

```bash
make .
```

#### Running tests

Testing is automated with a list of different parameters for mergesort.c. It will display timings to evaluate performance.

```bash
./tester.sh // builds and tests
```

**Visualising performance**
To plot the timing for your tests, run

```bash
pip install matplotlib
python plot_performance.py
```

## Features and usage

Summarise the main features of your program. It is also appropriate to
instruct the user how to use your program.

## Testing

The code was initially tested using the tester.sh script to test a variety of valid inputs and invalid inputs.
the plot_performance.py runs the testing script provided and graphs the time it took to execute, these findings were then used to limit
the number of threads allowed to run at any given time, with the goal of optimising execution time.


## Known Bugs

No bugs have been found from our testing

## Reflection and Self Assessment

Discuss the issues you encountered during development and testing. What
problems did you have? What did you have to research and learn on your own?
What kinds of errors did you get? How did you fix them?

What parts of the project did you find challenging? Is there anything that
finally "clicked" for you in the process of working on this project? How well
did the development and testing process go for you?

#### Issues

The first issue encountered was not knowing how to implement mergesort - which was quickly fixed from some rudimentary research online. Then, it was understanding how to use the Threads API - which was fixed by reading the referenced documentation on the assignment page (and the information provided on the assignment page itself). Then, it was actually implementing the parallel mergesort, which required some back and forth with debugging, finding more bugs, then debugging again - a common process for learning new implementations.

#### Findings

## Sources Used

If you used any sources outside of the textbook, you should list them here.
If you looked something up on stackoverflow.com or you use help from AI, and
fail to cite it in this section, it will be considered plagiarism and dealt
with accordingly. So be safe CITE!
