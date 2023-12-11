CC = gcc
INCLUDE_DIRS= -Iinclude/ -Isubmodules/termcolor-c/include
CFLAGS = -Wall -Werror $(INCLUDE_DIRS)
SRC = src/main.c src/kill.c src/args.c src/read.c src/print.c src/hide.c
TARGET = badguy

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm -f $(TARGET)

