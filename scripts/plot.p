set terminal png
set logscale x 2

set title "Blue Gene Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/bgtotalexec.png'
plot 'graph0.dat' using 1:2 with linespoints title '1024 vertices', \
     'graph0.dat' using 1:3 with linespoints title '4096 vertices', \
     'graph0.dat' using 1:4 with linespoints title '8192 vertices', \
     'graph0.dat' using 1:5 with linespoints title '11264 vertices'

set title "Kratos Methods Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/kratosmethods.png'
plot 'graph1.dat' using 1:2 with linespoints title 'Clustering', \
     'graph1.dat' using 1:3 with linespoints title 'Distance', \
     'graph1.dat' using 1:4 with linespoints title 'Round Robin'


set title "Kratos Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/kratostotalexec.png'
plot 'graph2.dat' using 1:2 with linespoints title '4096 vertices', \
     'graph2.dat' using 1:3 with linespoints title '8192 vertices'


set title "Blue Gene Transfer vs. Compute"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/bgtransfercompute.png'
plot 'graph3.dat' using 1:2 with linespoints title 'Compute Time', \
     'graph3.dat' using 1:3 with linespoints title 'Transfer Time'
