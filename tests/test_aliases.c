#include "minunit.h"
#include "../aliases_basic.c"

static char* test_alias_in () {
	alias_arg arg;
	arg.msg = calloc(512, sizeof(char));
	strcpy(arg.msg, "`in");
	
	char* response;
	response = alias_in(&arg);
	mu_assert("`in responded to blank command", response == NULL);
	
	strcpy(arg.msg, "`in 1d1h1m1s");
	response = alias_in(&arg);
	mu_assert("`in responded to command missing parameter", response == NULL);
	
	memset(arg.msg, 0, 512 * sizeof(char));
	strcpy(arg.msg, "`in 1m say");
	response = alias_in(&arg);
	mu_assert("`in failed for 1 time arg", strcmp("Timer set for 1 minute.", response) == 0);
	
	strcpy(arg.msg, "`in 10d5h1m500s say");
	response = alias_in(&arg);
	mu_assert("`in failed for all time args", strcmp("Timer set for 10 days, 5 hours, 9 minutes and 20 seconds.", response) == 0);
	
	free(arg.msg);
	return 0;
}

static char* test_aliases () {
	mu_run_test(test_alias_in);

	return 0;
}
