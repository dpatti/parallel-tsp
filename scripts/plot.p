set terminal png
set logscale x 2
set logscale y 2

set title "Blue Gene Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set autoscale ymax fix 1024
set output 'graph/png/bgtotalexec.png'
plot 'graph/graph0.dat' using 1:2 with linespoints title '1024 vertices', \
     'graph/graph0.dat' using 1:3 with linespoints title '4096 vertices', \
     'graph/graph0.dat' using 1:4 with linespoints title '8192 vertices', \
     'graph/graph0.dat' using 1:5 with linespoints title '11264 vertices'
unset autoscale ymax

set title "Kratos Methods Cores vs. Total Exec (4096 vertices)"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/png/kratosmethods.png'
plot 'graph/graph1.dat' using 1:2 with linespoints title 'Clustering', \
     'graph/graph1.dat' using 1:3 with linespoints title 'Distance', \
     'graph/graph1.dat' using 1:4 with linespoints title 'Round Robin'

set title "Kratos Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/png/kratostotalexec-logscale.png'
plot 'graph/graph2.dat' using 1:2 with linespoints title '4096 vertices', \
     'graph/graph2.dat' using 1:3 with linespoints title '8192 vertices'

set title "Kratos Cores vs. Total Exec"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
unset logscale y
set output 'graph/png/kratostotalexec.png'
plot 'graph/graph2.dat' using 1:2 with linespoints title '4096 vertices', \
     'graph/graph2.dat' using 1:3 with linespoints title '8192 vertices'
set logscale y 2

set title "Blue Gene Blocking vs. Compute"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/png/bgtransfercompute.png'
plot 'graph/graph3.dat' using 1:2 with linespoints title 'Compute Time', \
     'graph/graph3.dat' using 1:3 with linespoints title 'Blocking Time'

set title "Kratos Blocking vs. Compute"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/png/kratostransfercompute.png'
plot 'graph/graph4.dat' using 1:2 with linespoints title 'Compute Time', \
     'graph/graph4.dat' using 1:3 with linespoints title 'Blocking Time'

set title "Blue Gene Virtual Node vs. Real Mode"
set xlabel "Number of Cores"
set ylabel "Execution time (s)"
set output 'graph/png/bgvntime.png'
plot 'graph/graph5.dat' using 1:2 with linespoints title 'VN Mode', \
     'graph/graph5.dat' using 1:3 with linespoints title 'Normal Mode'
