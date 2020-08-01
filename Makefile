#this was tested on Ubuntu 18.04.4 LTS

CC=gcc
src=src/*.c
inc=include/

all :
	${CC} ${src} -I${inc} -o ./run
