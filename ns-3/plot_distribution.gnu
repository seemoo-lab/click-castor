#set term tikz size 14, 7
set term svg size 1200,800
if (exists("outfile")) set output outfile

set style fill transparent solid 0.3 border lt -1
set linetype 1 lc rgb "#666666" lt -1

binwidth=0.001 # the width of each box
bin(x,width)=width*floor(x/width)

#unset key

set yrange [0:]
set xrange [0:]

set ylabel 'frequency'
set xlabel metric

# $x indicates the xth column in the file
plot filename using (bin($1,binwidth) + binwidth/2):(1.0) smooth freq with boxes title protocol

if (!exists("outfile")) pause -1
