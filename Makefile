CC=gcc
CFLAGS=-Wall -Wextra
TARGET=lms
MAIN=main.c
OBJS=ui.o library.o login.o utils.o

all: $(TARGET)

$(TARGET): $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)

library.o : utils.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
clean: 
	rm $(TARGET) $(OBJS)
