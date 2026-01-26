CC = gcc
CFLAGS = -Iheaders -Wall -Wextra
OBJ = output/server.o output/server_utils.o

all: output/main

output/main: $(OBJ)
	$(CC) $(OBJ) -o output/main

output/server.o: server/src/server.c
	$(CC) $(CFLAGS) -c server/src/server.c -o output/server.o

output/server_utils.o: server/src/server_utils.c
	$(CC) $(CFLAGS) -c server/src/server_utils.c -o output/server_utils.o

clean:
	rm -f output/*.o output/main