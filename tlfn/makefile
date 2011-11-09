# Macros do makefile
EXECUTABLE = tlfn
OBJECTS = tlfn.o
ifdef DEBUG
  CFLAGS = -g -pg -Wall
else
  CFLAGS = -O6 -Wall
endif
LIBRARIES = -lm
INC_DIR = -I./
LIB_DIR = -L./
CC = gcc


# Criacao do executavel (linker)
$(EXECUTABLE):	$(OBJECTS)
	$(CC) $(LIB_DIR) -o $(EXECUTABLE) $(OBJECTS) $(LIBRARIES) $(CFLAGS)


# Criacao dos objetos (.o)
.c.o:
	$(CC) $(INC_DIR) -c $< $(CFLAGS)

.cpp.o:
	$(CC) $(INC_DIR) -c $< $(CFLAGS)


# Clausula all
all: 	clean $(EXECUTABLE)


# Clausula clean
clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
