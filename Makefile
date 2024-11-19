CC=gcc
CFLAGS=-Wall -Wextra
LFLAGS=-lm
TARGET=lms
MAIN=main.c
OBJS=ui.o library.o login.o utils.o fuzzy.o preprocess.o soundex.o synonyms.o damerau-levenshtein.o rsa.o search-utils.o chatbot.o chat-utils.o

all: $(TARGET)

$(TARGET): $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS) $(LFLAGS)

library.o : utils.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(LFLAGS)
clean: 
	rm $(TARGET) $(OBJS)
