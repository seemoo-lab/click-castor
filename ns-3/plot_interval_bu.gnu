#set term tikz size 14, 7
set term svg size 1200,800
if (exists("outfile")) set output outfile

file_bu(what) = filename.'-'.what.'-interval'

set yrange [0:]

set ylabel 'bandwidth utilization per pid [bytes]'
set xlabel 'time'
set datafile separator ","

set linetype 1 lc rgb "#999999" lw 1
set linetype 2 lc rgb "#2f599f" lw 1
set linetype 3 lc rgb "#4582e7" lw 1
set linetype 4 lc rgb "#9f2f2f" lw 1
set linetype 5 lc rgb "#e74545" lw 1

titles = "Unicast_PKT Broadcast_PKT Broadcast_ACK Unicast_ACK"
plot \
  file_bu('bu_phy') with filledcurve x1 title 'PHY', \
  for [i=1:4:1] \
    file_bu('bu') using (sum [col=i:4] column(col)) \
      with filledcurve x1 title word(titles, i)

if (!exists("outfile")) pause -1
