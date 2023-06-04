CC = gcc
CFLAGS = -Iinclude
EXEC= ./build/pips
INSTALLDIR= /usr/local/bin

build/pips: build build/main.o include/config.h include/config.h build/exec.o build/parser.o build/utils.o build/defs.o build/reader.o build/hashmaps.o include/types.h
	$(CC) build/main.o build/exec.o build/parser.o build/utils.o build/defs.o build/reader.o build/hashmaps.o -lm -o $(EXEC)

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

install: build/pips
	@echo "Installing Pipescript"
	@echo "Installing in $(INSTALLDIR)"
	cp -f $(EXEC) $(INSTALLDIR)/pips
	chmod 755 $(INSTALLDIR)/pips

remove:
	@echo "Removing Pipescript"
	rm $(INSTALLDIR)/pipescript

test:
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         $(EXEC) test.pipescript

run:
	@echo "------------------"
	@$(EXEC) test.pipescript
	@echo "------------------"

clean:
	@rm -rf build
