CC=gcc
CFLAGS=-I. -lrt -pthread
DEPS = cJSON.h socket.h
SERVER_OBJ = server.o slog.o cJSON.o socket_send.o
CLIENT_OBJ = client.o slog.o socket_send.o cJSON.o
BEACON_ENROLL_OBJ = beacon_enroll.o cJSON.o slog.o beacon_scan.o socket_send.o
all: client server beacon_enroll
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
client: $(CLIENT_OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)


server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

beacon_enroll: $(BEACON_ENROLL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm -f server client beacon_enroll
