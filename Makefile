CC=g++
CFLAGS=-I.
DEPS = hellomake.h
OBJ = hiking_trip.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hiking_trip: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -pthread
