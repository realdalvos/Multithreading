# call:
# prompt> gnuplot speedup.gnuplot

set terminal png
set output "speedup.png"
set grid

set xlabel "number of threads"
set ylabel "Speedup"
set key autotitle columnheader

plot \
     "speedup.dat" using 1:2 ls 1 with linespoints, \
     "speedup.dat" using 1:3 ls 2 with linespoints, \
     "speedup.dat" using 1:4 ls 3 with linespoints, \
     "speedup.dat" using 1:5 ls 4 with linespoints, \
     "speedup.dat" using 1:6 ls 5 with linespoints, \
     "speedup.dat" using 1:7 ls 6 with linespoints, \
     "speedup.dat" using 1:8 ls 7 with linespoints, \
     "speedup.dat" using 1:9 ls 8 with linespoints

