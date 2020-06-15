CC=gcc
CFLAGS=-I.
DEPS =
SERVER_OBJ = server.o
CLIENT_OBJ = client.o

all: client server
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
client: $(CLIENT_OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)


server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


	
clean:
	rm -f server client