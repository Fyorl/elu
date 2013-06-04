#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alias_map.h"
#include "connection.h"
#include "config.h"
#include "elu.h"
#include "file_utils.h"
#include "irc.h"
#include "string_utils.h"
#include "threadpool.h"

#define CONFIG_FILE "bot.conf"

#define TRUE 1
#define FALSE 0
#define RECEIVE_BUFFER_SIZE 1024

extern int sock;
extern config_t* config;
extern hashmap_t alias_map;
extern sqlite3* db;

int quit (const char* string) {
	int pos_colon = strpos(string + 1, ":");
	int pos_privmsg = strpos(string, "PRIVMSG");
	int pos_bang = strpos(string, "!");
	int pos_quit = strpos(string, "`quit");

	char* nick;

	if (pos_colon > -1 && pos_privmsg > -1 && pos_privmsg < pos_colon && pos_quit == pos_colon + 2) {
		nick = calloc(pos_bang, sizeof(char));
		memmove(nick, string + 1, (pos_bang - 1) * sizeof(char));
		nick[pos_bang - 1] = '\0';

		if (hashmap_get(&(config->admins), nick) != NULL) {
			free(nick);
			return 1;
		}

		free(nick);
	}

	return 0;
}

void init_db () {
	int file_error;
	int sql_error;
	FILE* db_file;

	// Check if the database has any data in, otherwise we must create the
	// tables.
	struct stat file_info;
	file_error = stat("db/main.sl3", &file_info);

	if (file_error < 0) {
		// File possibly doesn't exist. We should try to create it.
		db_file = fopen("db/main.sl3", "w+");
		fclose(db_file);

		if (db_file == NULL) {
			fprintf(stderr, "Database creation failed due to lack of permissions.\n");
		}
	}

	sql_error = sqlite3_open("db/main.sl3", &db);

	if (sql_error) {
		fprintf(stderr, "Unable to open database.\n");
	}

	stat("db/main.sl3", &file_info);
	if (file_info.st_size > 0) {
		return;
	}

	char* tbl_timers = file_get_contents("db/timers.sql");
	sql_error = sqlite3_exec(db, tbl_timers, NULL, NULL, NULL);
	free(tbl_timers);
}

int main (int argc, char** argv) {
	queue_t queue;
	queue_init(&queue, sizeof(char*), 1);

	// We only want a 'threadpool' of size one as we still want to process
	// messages in order but we don't want our main data receiving loop to have
	// to wait for a message to be processed before reading from the socket.
	threadpool_t threadpool;
	threadpool_init(&threadpool, 1, &queue, &irc_handle_chunk);

	config_t config_in;
	config = &config_in;
	read_config(config, CONFIG_FILE);

	map_aliases(&alias_map);
	init_db();

	sock = establish_connection(config->host, config->port);

	int bytes_received;
	char bytes_in[RECEIVE_BUFFER_SIZE];
	char* work;

	while (TRUE) {
		// Receive up to RECEIVE_BUFFER_SIZE bytes of data from the server
		bytes_received = recv(sock, bytes_in, RECEIVE_BUFFER_SIZE - 1, 0);

		if (bytes_received < 1) {
			printf("Connection terminated. Shutting down.\n");
			break;
		}

		// Make the last character a null character so that printf can print
		// the whole thing as a string.
		bytes_in[bytes_received] = '\0';
		printf("%s", bytes_in);

		// Flush standard output so we see the strings printed as soon as we
		// get them from the server.
		fflush(stdout);

		// Have to hack in the `quit command here since implementing it through
		// the proper pipeline is way more effort than necessary.
		// We only really want this for valgrind anyway so we can remove it
		// from production.
		if (quit(bytes_in)) {
			break;
		}

		// Copy the data into some heap memory to pass to the threadpool to
		// deal with.
		work = calloc(bytes_received + 1, sizeof(char));
		strncpy(work, bytes_in, bytes_received + 1);
		threadpool_add_work(&threadpool, &work);
	}

	sqlite3_close(db);
	config_destroy(config);
	threadpool_destroy(&threadpool);
	hashmap_destroy(&alias_map);
	return EXIT_SUCCESS;
}
