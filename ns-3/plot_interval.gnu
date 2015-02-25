#set term tikz size 14, 7
set term svg size 1200,800
if (exists("outfile")) set output outfile

set linetype 1 lc rgb "#000000" lw 1
set linetype 2 lc rgb "#2f599f" lw 1
set linetype 3 lc rgb "#9f2f2f" lw 1

set yrange [0:]

set ylabel metric
set xlabel 'time'

plot filename with line title protocol

if (!exists("outfile")) pause -1
