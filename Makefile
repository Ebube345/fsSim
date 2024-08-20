PROG= fs
OBJS= bintree.o simulator.o main.o
CFLAGS= -Wall -Wextra -g

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

all: ${PROG}

bintree.o: include/bintree.h
main.o: include/bintree.h 

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} ${OBJS} -o ${PROG} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}

help: ## Prints help for targets with comments
