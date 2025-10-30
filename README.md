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

## Features and usage

Summarise the main features of your program. It is also appropriate to
instruct the user how to use your program.

## Testing

This section should detail how you tested your code. Simply stating "I ran
it a few times and it seems to work" is not sufficient. Your testing needs
to be detailed here.

## Known Bugs

List known bugs that you weren't able to fix (or ran out of time to fix).

## Reflection and Self Assessment

Discuss the issues you encountered during development and testing. What
problems did you have? What did you have to research and learn on your own?
What kinds of errors did you get? How did you fix them?

What parts of the project did you find challenging? Is there anything that
finally "clicked" for you in the process of working on this project? How well
did the development and testing process go for you?

## Sources Used

If you used any sources outside of the textbook, you should list them here.
If you looked something up on stackoverflow.com or you use help from AI, and
fail to cite it in this section, it will be considered plagiarism and dealt
with accordingly. So be safe CITE!
