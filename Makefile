CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = src/common.c src/client.c src/server.c src/client_protocol.c src/server_protocol.c
OBJ = $(SRC:src/%.c=build/%.o)

CLIENT_MAIN = build/main_client.o
SERVER_MAIN = build/main_server.o

BIN_CLIENT = client
BIN_SERVER = server

all: dirs $(BIN_CLIENT) $(BIN_SERVER)

dirs:
	mkdir -p build

$(BIN_CLIENT): $(CLIENT_MAIN) $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_SERVER): $(SERVER_MAIN) $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN_CLIENT) $(BIN_SERVER)
