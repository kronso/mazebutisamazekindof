CC = g++

CFILES = a_star.cpp maze.cpp maze.hpp timer.hpp
OBJECTS = a_star.o maze.o

BINARY = bin

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $^
clean:
	del $(BINARY) $(OBJECTS)