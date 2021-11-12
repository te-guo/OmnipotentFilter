CFLAGS = -g -Wall -std=c++14 -mpopcnt
SRC = ./*.cpp ../include/*.cpp

all: test

test: $(SRC)
	g++ $(CFLAGS) -Ofast -o test -I. -I../include $(SRC)

clean:
	rm -f test
