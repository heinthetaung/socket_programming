CC=gcc
CFLAGS=-I. -lrt -pthread -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -lmongoc-1.0 -lbson-1.0
DEPS = cJSON.h socket.h db.h logging.h
SERVER_OBJ = server.o slog.o cJSON.o socket_send.o db.o logging.o
CLIENT_OBJ = client.o slog.o socket_send.o cJSON.o db.o logging.o
MGMT_CLIENT_OBJ = mgmt_client.o slog.o socket_send_mgmt.o cJSON.o db.o logging.o
BEACON_ENROLL_OBJ = beacon_enroll.o cJSON.o slog.o beacon_scan.o socket_send.o db.o logging.o
all: mgmt_client client server beacon_enroll
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
client: $(CLIENT_OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)

mgmt_client: $(MGMT_CLIENT_OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)

server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

beacon_enroll: $(BEACON_ENROLL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
	
clean:
	rm -f server client mgmt_client beacon_enroll *.o
