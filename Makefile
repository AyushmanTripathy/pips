CC = gcc
CFLAGS = -Iinclude

bulid/main: build build/main.o build/utils.o build/defs.o build/reader.o build/hashmaps.o include/types.h
	$(CC) build/main.o build/utils.o build/defs.o build/reader.o build/hashmaps.o -o build/main

build/main.o: src/main.c include/main.h
	$(CC) $(CFLAGS) src/main.c -c -o build/main.o

build/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) src/utils.c -c -o build/utils.o

build/defs.o: src/defs.c include/defs.h
	$(CC) $(CFLAGS) src/defs.c -c -o build/defs.o

build/reader.o: src/reader.c include/reader.h
	$(CC) $(CFLAGS) src/reader.c -c -o build/reader.o

build/hashmaps.o: src/hashmaps.c include/hashmaps.h
	$(CC) $(CFLAGS) src/hashmaps.c -c -o build/hashmaps.o

build:
	mkdir build

run:
	./build/main test.pipescript

dev:
	nodemon -w src/* -w include/* -w test.pipescript -e "c h" -x "make && ./build/main test.pipescript"

clean:
	@rm -rf build
