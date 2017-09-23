CC = clang++

CFLAGS = -std=c++14 -Wall -c -g

all: interface.o som-module.o algo.o driver.o hjscript.o lam-kernel.o
	$(CC) interface.o som-module.o algo.o driver.o hjscript.o lam-kernel.o -o lamstar

algo.o: algo.cpp algo.hpp
	$(CC) $(CFLAGS) algo.cpp

som-module.o: som-module.cpp algo.hpp som-module.hpp
	$(CC) $(CFLAGS) som-module.cpp

lam-kernel.o: lam-kernel.cpp lam-kernel.hpp som-module.hpp algo.hpp
	$(CC) $(CFLAGS) lam-kernel.cpp

hjscript.o: hjscript.cpp hjscript.hpp
	$(CC) $(CFLAGS) hjscript.cpp

interface.o: interface.cpp interface.hpp lam-kernel.hpp som-module.hpp algo.hpp hjscript.hpp
	$(CC) $(CFLAGS) interface.cpp

driver.o: driver.cpp interface.hpp lam-kernel.hpp som-module.hpp algo.hpp hjscript.hpp
	$(CC) $(CFLAGS) driver.cpp

clean:
	$(RM) huaji *.o *~