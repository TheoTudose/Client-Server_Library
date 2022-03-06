CC=g++
CFLAGS=-I. -Wall -Wextra -std=c++17
DEPS=request.h actions.h commands.h json.h
OBJ=request.o actions.o commands.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: client.o $(OBJ)
	$(CC) -o $@ client.o $(OBJ)

.PHONY: clean

clean:
	rm -f *.o