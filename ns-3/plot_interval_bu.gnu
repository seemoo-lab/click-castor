#set term tikz size 14, 7

exp = 'test'
dir = '/home/milan/'

file_bu(what) = dir.exp.'-'.what.'-interval'
file_bu_phy = file_bu('bu_phy')
file_bu_net = file_bu('bu')
file_bu_pkt = file_bu('bu_pkt')
file_bu_ack = file_bu('bu_ack')

set yrange [0:]

set ylabel 'bandwidth utilization per pid [bytes]'
set xlabel 'time'

# $x indicates the xth column in the file
plot file_bu_phy with line lt -1 lw 2 title 'PHY', \
     file_bu_net with filledcurve x1 lc rgb "#AAAAAA" title 'PKT', \
     file_bu_ack with filledcurve x1 lc rgb "#666666" title 'ACK', \
     file_bu_net with line lt -1 lw 2 lc rgb "#000000" title 'NET'

pause -1
