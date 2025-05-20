CC = gcc
CFLAGS = -Wall -pedantic -g -DUNIX_HOST
OBJ = table.o lex.o parse.o expression.o heap.o type.o variable.o clibrary.o platform.o include.o platform/platform_unix.o cstdlib/stdio.o cstdlib/math.o cstdlib/string.o cstdlib/stdlib.o cstdlib/time.o cstdlib/errno.o cstdlib/ctype.o


TARGET = picoc.a
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after


clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	ar qc $(TARGET) $(OBJ)

table.o: table.c
	$(CC) -c table.c -o table.o $(CFLAGS)

lex.o: lex.c
	$(CC) -c lex.c -o lex.o $(CFLAGS)

parse.o: parse.c
	$(CC) -c parse.c -o parse.o $(CFLAGS)

expression.o: expression.c
	$(CC) -c expression.c -o expression.o $(CFLAGS)

heap.o: heap.c
	$(CC) -c heap.c -o heap.o $(CFLAGS)

type.o: type.c
	$(CC) -c type.c -o type.o $(CFLAGS)

variable.o: variable.c
	$(CC) -c variable.c -o variable.o $(CFLAGS)

clibrary.o: clibrary.c
	$(CC) -c clibrary.c -o clibrary.o $(CFLAGS)

platform.o: platform.c
	$(CC) -c platform.c -o platform.o $(CFLAGS)

include.o: include.c
	$(CC) -c include.c -o include.o $(CFLAGS)

platform/platform_unix.o: platform/platform_unix.c
	$(CC) -c platform/platform_unix.c -o platform/platform_unix.o $(CFLAGS)

cstdlib/stdio.o: cstdlib/stdio.c
	$(CC) -c cstdlib/stdio.c -o cstdlib/stdio.o $(CFLAGS)

cstdlib/math.o: cstdlib/math.c
	$(CC) -c cstdlib/math.c -o cstdlib/math.o $(CFLAGS)

cstdlib/string.o: cstdlib/string.c
	$(CC) -c cstdlib/string.c -o cstdlib/string.o $(CFLAGS)

cstdlib/stdlib.o: cstdlib/stdlib.c
	$(CC) -c cstdlib/stdlib.c -o cstdlib/stdlib.o $(CFLAGS)

cstdlib/time.o: cstdlib/time.c
	$(CC) -c cstdlib/time.c -o cstdlib/time.o $(CFLAGS)

cstdlib/errno.o: cstdlib/errno.c
	$(CC) -c cstdlib/errno.c -o cstdlib/errno.o $(CFLAGS)

cstdlib/ctype.o: cstdlib/ctype.c
	$(CC) -c cstdlib/ctype.c -o cstdlib/ctype.o $(CFLAGS)
