
#This addition first creates the macro DEPS, which is the set of .h files on which the .c files depend.
#Then we define a rule that applies to all files ending in the .o suffix. The rule says that
#the .o file depends upon the .c version of the file and the .h files included in the DEPS macro.
#The rule then says that to generate the .o file, make needs to compile the .c file using the compiler defined in the CC macro.
# The -c flag says to generate the object file, the -o $@ says to put the output of the compilation in the file
#named on the left side of the :, the $< is the first item in the dependencies list, and the CFLAGS macro is defined as above.
#As a final simplification, let's use the special macros $@ and $^, which are the left and right sides of the :,
#respectively, to make the overall compilation rule more general. In the example below, all of the include files should
# be listed as part of the macro DEPS, and all of the object files should be listed as part of the macro OBJ.
CC=gcc
CFLAGS=-g
DEPS =

#names of source files
SRC = mreplace.c

#turn all .c into .o
#OBJ = $(SRC:.c=.o)

#remove .c from all SRC files
BASENAMES = $(SRC:.c=)
ECHO =echo -e

#Addd some color
NO_COLOR=\x1b[0m
OK_COLOR=\x1b[32;01m
ERROR_COLOR=\x1b[31;01m
WARN_COLOR=\x1b[33;01m

#some strings
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

#.PHONY declaration just documents that all isn't a file name.
.PHONY: all
all: $(BASENAMES) alloc.so


#this will compile our .so ...this means your malloc source must be named alloc.c
#....or change alloc.c to the name of your source files
alloc.so: alloc.c
	$(CC) $^ $(FLAGS) -o $@ -shared -fPIC

#compile all .o into executable
%: %.o $(DEPS)
	$(CC) -o $@ $<


#So if you had a program you could run it using your
#Malloc, by doing ./mreplace /usr/bin/ls


#..or you could uncomment the lines below and just type make run
#run tests:
#    ./mreplace ./test-1
#    ./mreplace ./test-2


#https://www.gnu.org/software/make/manual/html_node/Cleanup.html#Cleanup
.PHONY: clean
clean:
	-rm -f *.o alloc.so $(BASENAMES)
	@$(ECHO) '$(WARN_COLOR)Clean Done$(NO_COLOR)'