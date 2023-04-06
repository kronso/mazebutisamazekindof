CC = g++

LIBS = -lglfw3 -lglfw3 -lgdi32
CFILES = main.cpp
OBJECTS = main.o

BINARY = bin

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) -c -o $@ $^

clean:
	del $(OBJECTS) $(BINARY)