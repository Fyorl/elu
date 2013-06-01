elu
===

Impsvillage IRC bot

Rename example.conf to bot.conf and modify as desired.

Compile with the following for a debug build:
```$ gcc -Wall -Werror -ggdb -o elu *.c vendor/*.c -lpthread -ldl```

Or with the following for a release build:
```$ gcc -s -O3 -o elu *.c vendor/*.c -lpthread -ldl```

To compile the tests, run the following in the tests/ directory:
```$ gcc -Wall -Werror -ggdb -o run run.c -lpthread -ldl```
