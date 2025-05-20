CC = gcc
CFLAGS = -Wall -pedantic -g -DUNIX_HOST

OBJ = main.o loadImg.o picoc_modified/picoc.a
LIBS = -lm -lreadline -lpthread -lGLEW -lGL -lglfw -ldl

BIN = prog

.PHONY: $(BIN) subdir clean clean-custom

all: subdir $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) $(INCS) -c main.c -o main.o

loadImg.o: loadImg.c
	$(CC) -c loadImg.c -o loadImg.o $(CFLAGS)

subdir:
	$(MAKE) -C picoc_modified

clean: clean-custom
	${RM} $(OBJ) $(BIN)

clean-custom:
	$(MAKE) -C picoc_modified clean
