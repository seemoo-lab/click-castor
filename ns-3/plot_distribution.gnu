#set term tikz size 14, 7

set style fill transparent solid 0.3 border lt -1
set linetype 1 lc rgb "#666666" lt -1

#what = 'delay'
#what = 'neighbors'
what = 'hopcount'
exp = 'test'
dir = '/home/milan/'

file = dir.exp.'-'.what.'-dist'

binwidth=0.001 # the width of each box
bin(x,width)=width*floor(x/width)

unset key

set yrange [0:]

set ylabel 'frequency'
set xlabel what

# $x indicates the xth column in the file
plot file using (bin($1,binwidth) + binwidth/2):(1.0) smooth freq with boxes

pause -1
