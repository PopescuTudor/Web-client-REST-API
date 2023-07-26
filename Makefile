CC=g++
CFLAGS=-I.

client: client.cpp
	$(CC) -o client client.cpp buffer.cpp requests.cpp helpers.cpp -w

run: client
	./client

clean:
	rm -f *.o client
