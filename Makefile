.PHONY: test debug release

LINKS = -lpthread -ldl
DEBUGFLAGS = -Wall -Werror -ggdb
RELEASEFLAGS = -s -O3

test:
	cd tests
	gcc $(DEBUGFLAGS) -o run run.c ../vendor/city.c $(LINKS)
	./run
	cd ..

debug:
	gcc $(DEBUGLFAGS) -o elu *.c vendor/*.c $(LINKS)

release:
	gcc $(RELEASEFLAGS) -o elu *.c vendor/*.c $(LINKS)

