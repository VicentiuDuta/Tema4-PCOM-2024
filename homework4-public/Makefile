CC = g++
CCFLAGS = -Wall -Wextra -std=c++17 -O0 -lm

.PHONY: build clean

build: client

pack:
	zip -r archive.zip Makefile README.md client.cpp buffer.cpp helpers.cpp requests.cpp

run:
	./client

client: client.cpp buffer.cpp helpers.cpp requests.cpp
	$(CC) -o $@ $^ $(CCFLAGS)

clean:
	rm -f client
