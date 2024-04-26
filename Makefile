CC = gcc
CFLGGS = -Wall -g -std=c99 -Werror

## All: run the prog target
.PHONY: All
All: final

## prog: link all the .o file dependencies to create the executable
final: bonus.o program.o print.o
	$(CC) $(CFLAGS) -o $@ $^

##%.o: compile all .c files to .o files
%.o: %.c header.h
	$(CC) $(CFLAGS) -c $<

## clean : remove all object files and executable and output files
.PHONY: clean
clean:
	rm -f *.o final
	rm -f composite*
