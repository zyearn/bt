set term png
set output 'simple.1.png'
set key left bottom box
set yr [0.6:1.05]
set ylabel 'recall'
set xlabel 'The percentage of the modified files'

plot "./data" using 1:2 title 'ε=0' with linespoints, \
    "./data" using 1:3 title 'ε=0.4' with linespoints, \
    "./data" using 1:4 title 'Hash based method' with linespoints

