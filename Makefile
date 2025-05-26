CC = gcc
CFLAGS = -Wall -pedantic -g -DUNIX_HOST

OBJ = main.o loadImg.o audio.o mp3.o picoc_modified/picoc.a
LIBS = -lm -lreadline -lpthread -lGLEW -lGL -lglfw -ldl -lmp3lame

BIN = prog

.PHONY: $(BIN) subdir clean clean-custom

all: subdir $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) $(INCS) -c main.c -o main.o

loadImg.o: loadImg.c
	$(CC) -c loadImg.c -o loadImg.o $(CFLAGS)

audio.o: audio.c
	$(CC) -c audio.c -o audio.o $(CFLAGS)

mp3.o: mp3.c
	$(CC) -c mp3.c -o mp3.o $(CFLAGS)

subdir:
	$(MAKE) -C picoc_modified

clean: clean-custom
	${RM} $(OBJ) $(BIN)

clean-custom:
	$(MAKE) -C picoc_modified clean
