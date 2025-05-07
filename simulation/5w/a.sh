cc -o $1.o $1.c
args=("$@")
./$1.o "${args[@]:1}" > plot.txt
rm $1.o