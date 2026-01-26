CC = gcc
CFLAGS = -Iheaders -Wall -Wextra
OBJ = output/server.o output/server_utils.o

all: output/main

output:
	mkdir -p output

output/main: output $(OBJ)
	$(CC) $(OBJ) -o output/main

output/server.o: server/src/server.c | output
	$(CC) $(CFLAGS) -c server/src/server.c -o output/server.o

output/server_utils.o: server/src/server_utils.c | output
	$(CC) $(CFLAGS) -c server/src/server_utils.c -o output/server_utils.o

clean:
	rm -f output/*.o output/main