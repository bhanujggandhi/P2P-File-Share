CC = g++
CFLAGS = -W
DEPS = utils.h
OBJ = tracker.o utils.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

tracker: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lcrypto -lpthread
 
clean:
	rm -rf *o tracker