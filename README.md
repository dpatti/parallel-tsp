Parallel Ant Colony Optimization for TSP
========================================

Doug Patti, Ziv Kennan, and Patrick Phipps

Abstract
--------

This project provides a look into the thought processes, considerations, results
and implementation details of our final project: parallelizing the ant colony
optimization to solve the traveling salesman problem within a reasonable bound.

It then provides a discussion on the results of the modified algorithm,
comparing it to a serial solution as well as studying the scaling properties on
two different computer systems. The two systems used in this study will be an
optimized high perfomance system called Kratos and a Blue Gene/L system here at
RPI.

Requirements
------------

* gcc or equivalent
* MPICH2
* Python (optional): prototype ACO implementation
* Ruby (optional): result compilation)
* gnuplot (optional): graph generation
* pdflatex (optional): report compilation


Directory Structure
-------------------

* docs: Spreadsheets used for calculations and results
* graph: Output data and image files for graphs
* out: Collection of raw ouput data
* scripts: Various shell, ruby, and python scripts used along the line. Also
  contains python implementation of ACO.
* src: Source and header files for C implementation
* tex: Report in raw format

Makefile Commands
-----------------

* (all): Builds the program
* bluegene: Builds using parameters for the BlueGene/L
* queue: Adds a script to the SLURM queue
* results: Prints results of latest BlueGene run
* run/multicore/multiant/full/verbose/kratos: Various run configurations for testing
* pdf: Create report document
* parse: Read output data and format as tabulated columns
* compile: Read edited parsed data and create graphs
* clean: Remove temporary files from directory
* fclean: In addition to clean, also removes binaries

License
-------

The MIT License

Copyright (c) 2012 Doug Patti, Ziv Kennan, and Patrick Phipps

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
