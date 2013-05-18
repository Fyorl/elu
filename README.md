elu
===

Impsvillage IRC bot

Rename example.conf to bot.conf and modify as desired.

Compile with the following for a debug build:
```$ gcc -Wall -Werror -ggdb -o elu elu.c config.c connection.c```

Or with the following for a release build:
```$ gcc -s -O3 -o elu elu.c config.c connection.c```
