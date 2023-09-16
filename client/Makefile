CC = g++
CFLAGS = -Wall 
DEPS = utils.h
OBJ = client.o utils.o server.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lcrypto
 
clean:
	rm -rf *o client