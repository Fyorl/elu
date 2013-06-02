#include "minunit.h"
#include "../config.c"

static char* test_read_config () {
	config_t config;
	read_config(&config, "../example.conf");

	mu_assert("config.host != 'irc.rizon.net'", strcmp("irc.rizon.net", config.host) == 0);
	mu_assert("config.nick != 'elu'", strcmp("elu", config.nick) == 0);
	mu_assert("config.pass != 'password'", strcmp("password", config.pass) == 0);
	mu_assert("config.port != 6667", config.port == 6667);
	mu_assert("config.channels[0] != '#impsvillage'", strcmp("#impsvillage", vector_get(config.channels, 0, char*)) == 0);

	// Free up stuff to make valgrind happy
	config_destroy(&config);

	return 0;
}

static char* test_config () {
	mu_run_test(test_read_config);
	return 0;
}
