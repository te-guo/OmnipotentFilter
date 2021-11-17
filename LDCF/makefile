<<<<<<< HEAD
CFLAGS = -g -Wall -std=c++17 -mpopcnt
SRC = ./*.cpp ../include/*.cpp
=======
CFLAGS = -g -Wall -std=c++14 -mpopcnt
SRC = ./*.cpp ../include/* ./*.h
>>>>>>> 8fadd529c05a9805060bdaf212b5fe6436daa5ae

all: test

test: $(SRC)
	g++ $(CFLAGS) -Ofast -o test -I. -I../include $(SRC)

clean:
	rm -f test
