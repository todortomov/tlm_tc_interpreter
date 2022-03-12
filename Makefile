CFLAGS ?= -Wall

all: telecommand_interpreter

telecommand_interpreter: telecommand_interpreter.o

.PHONY: clean
clean:
	rm -f *.o
	rm -f telecommand_interpreter
