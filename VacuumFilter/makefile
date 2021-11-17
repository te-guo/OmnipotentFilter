CFLAGS = -g -Wall -std=c++17 -mpopcnt
SRC = ./*.cpp ../include/*.cpp
HEADERS = ../include/evaluation.h ../include/hashing.h
all: test

test: $(SRC) $(HEADERS)
	g++ $(CFLAGS) -Ofast -o test -I../include $(SRC)

clean:
	rm -f test
