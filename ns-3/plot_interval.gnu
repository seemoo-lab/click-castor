#set term tikz size 14, 7

what = 'delay'
exp = 'test'
dir = '/home/milan/'

file = dir.exp.'-'.what.'-interval'

unset key

set yrange [0:]

set ylabel what
set xlabel 'time'

plot file with line lt -1 lw 2

pause -1
