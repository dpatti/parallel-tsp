set terminal png
set logscale x 2

set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set title "Blue Gene Cores vs. Total Exec"
set output 'graph/bgtotalexec.png'
plot 'results.dat' using 1:2 with linespoints title 'Time'


set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set title "Kratos Cores vs. Total Exec"
set output 'graph/kratostotalexec.png'
plot 'results.dat' using 1:4 with linespoints title 'Time Per Core'


set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set title "Kratos Methods Cores vs. Total Exec"
set output 'graph/kratosmethods.png'
plot 'results.dat' using 1:5 with linespoints title 'Time'


set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set title "Blue Gene Transfer vs. Compute"
set output 'graph/bgtransfercompute.png'
plot 'results.dat' using 1:8 with linespoints title 'Read/Write Time', 'results.dat' using 1:6 with linespoints title 'Read Time', 'results.dat' using 1:7 with linespoints title 'Write Time'
