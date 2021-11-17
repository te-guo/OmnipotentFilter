CFLAGS = -g -Wall -std=c++17 -mpopcnt
SRC = ./*.cpp ../include/*.cpp

all: test

test: $(SRC) ../include/evaluation.h ../include/hashing.h omnipotent.h
	g++ $(CFLAGS) -Ofast -o test -I. -I../include $(SRC)

clean:
	rm -f test
