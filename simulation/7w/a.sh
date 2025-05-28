cc -o $1.o $1.c
args=("$@")
./$1.o "${args[@]:1}" > plot.data
rm $1.o