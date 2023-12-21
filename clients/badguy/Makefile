CC = gcc
INCLUDE_DIRS= -Iinclude/ -Isubmodules/termcolor-c/include
CFLAGS = -Wall -Werror
SRC = src/main.c src/kill.c src/args.c src/read.c src/print.c src/hide.c
TARGET = badguy

$(TARGET): badguy
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm -f $(TARGET)

