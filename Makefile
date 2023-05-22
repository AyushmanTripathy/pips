CC = gcc
CFLAGS = -Iinclude
EXEC= ./build/main

bulid/main: build build/main.o build/exec.o build/parser.o build/utils.o build/defs.o build/reader.o build/hashmaps.o include/types.h
	$(CC) build/main.o build/exec.o build/parser.o build/utils.o build/defs.o build/reader.o build/hashmaps.o -lm -o build/main

build/main.o: src/main.c include/main.h
	$(CC) $(CFLAGS) src/main.c -c -o build/main.o

build/exec.o: src/exec.c include/exec.h
	$(CC) $(CFLAGS) src/exec.c -c -o build/exec.o

build/parser.o: src/parser.c include/parser.h
	$(CC) $(CFLAGS) src/parser.c -c -o build/parser.o

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

test:
	@sh test.sh $(EXEC)

run:
	@echo "------------------"
	@$(EXEC) test.pipescript
	@echo "------------------"

clean:
	@rm -rf build
