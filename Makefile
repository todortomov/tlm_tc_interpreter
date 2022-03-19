CFLAGS ?= -Wall

all: telecommand_interpreter

telecommand_interpreter: telecommand_interpreter.o id_generator.o command_list.o

telecommand_interpreter.o: telecommand_interpreter.c telecommand_limits.h

command_list.o: command_list.c telecommand_limits.h

id_generator.o: id_generator.c telecommand_limits.h

.PHONY: clean
clean:
	rm -f *.o
	rm -f telecommand_interpreter
