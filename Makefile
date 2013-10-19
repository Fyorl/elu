.PHONY: test debug release

LINKS = -lpthread -ldl
DEBUGFLAGS = -Wall -Werror -ggdb
RELEASEFLAGS = -s -O3

test:
	gcc $(DEBUGFLAGS) -o tests/run tests/run.c timestamp.c vendor/city.c $(LINKS)
	tests/run

debug:
	gcc $(DEBUGFLAGS) -o elu *.c vendor/*.c $(LINKS)

release:
	gcc $(RELEASEFLAGS) -o elu *.c vendor/*.c $(LINKS)

