cc -o $1.o $1.c
args=("$@")
./$1.o "${args[@]:1}"
rm $1.o