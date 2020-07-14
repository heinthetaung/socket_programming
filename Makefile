CC=gcc
CFLAGS=-I. -lrt -pthread
DEPS =
SERVER_OBJ = server.o slog.o
CLIENT_OBJ = client.o slog.o socket_send.o cJSON.o

all: client server
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
client: $(CLIENT_OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)


server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


	
clean:
	rm -f server client
