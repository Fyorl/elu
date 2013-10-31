.PHONY: test debug release

LINKS = -lpthread -ldl -lrt
FLAGS = -DSQLITE_THREADSAFE=1
DEBUGFLAGS = $(FLAGS) -Wall -Werror -ggdb
RELEASEFLAGS = $(FLAGS) -s -O3

test:
	gcc $(DEBUGFLAGS) -o tests/run tests/run.c timestamp.c irc.c timers.c vendor/city.c $(LINKS)
	tests/run

debug:
	gcc $(DEBUGFLAGS) -o elu *.c vendor/*.c $(LINKS)

release:
	gcc $(RELEASEFLAGS) -o elu *.c vendor/*.c $(LINKS)

win32:
	gcc $(FLAGS) -Wall -ggdb -o elu *.c vendor/*.c $(LINKS)
