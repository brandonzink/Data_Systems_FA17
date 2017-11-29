#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cassandra.h>
#include "config.h"


char *readline(char *prompt);

static int tty = 0;

static void
cli_about()
{
	printf("You executed a command!\n");
}

static void
cli_help()
{
	return;
}

void
cli()
{
	char *cmdline = NULL;
	char cmd[BUFSIZE], prompt[BUFSIZE];
	int pos;

	tty = isatty(STDIN_FILENO);
	if (tty)
		cli_about();

	//SET IT UP
	CassFuture* connect_future = NULL;
  	CassCluster* cluster = cass_cluster_new();
  	CassSession* session = cass_session_new();
  	char* hosts = "127.0.0.1";

  /* Add contact points */
  cass_cluster_set_contact_points(cluster, hosts);

  /* Provide the cluster object as configuration to connect the session */
  connect_future = cass_session_connect(session, cluster);


	/* Main command line loop */
	for (;;) {
		if (cmdline != NULL) {
			free(cmdline);
			cmdline = NULL;
		}
		memset(prompt, 0, BUFSIZE);
		sprintf(prompt, "cassandra> ");

		if (tty)
			cmdline = readline(prompt);
		else
			cmdline = readline("");

		if (cmdline == NULL)
			continue;

		if (strlen(cmdline) == 0)
			continue;

		if (!tty)
			printf("%s\n", cmdline);

		if (strcmp(cmdline, "?") == 0) {
			cli_help();
			continue;
		}
		if (strcmp(cmdline, "quit") == 0 ||
		    strcmp(cmdline, "q") == 0)
			break;

		memset(cmd, 0, BUFSIZE);
		pos = 0;
		nextarg(cmdline, &pos, " ", cmd);

		if (strcmp(cmd, "about") == 0 || strcmp(cmd, "a") == 0) {
			cli_about();
			continue;
		}

		if(strcmp(cmd, "show") == 0){
			//SHOW FUNCTION
			if (cass_future_error_code(connect_future) == CASS_OK) {
    			CassFuture* close_future = NULL;
				char result[BUFSIZE];
				printf("Entering show function.\n");
				const CassSchemaMeta* schema_meta = cass_session_get_schema_meta(session);
				CassIterator* iterator = cass_iterator_keyspaces_from_schema_meta(schema_meta);

				while(cass_iterator_next(iterator)){
					const CassRow* row = cass_iterator_get_row(iterator);
					const CassKeyspaceMeta* keyspace = cass_iterator_get_keyspace_meta(iterator);
					const char* name;
					size_t name_length;
					cass_keyspace_meta_name(keyspace, &name, &name_length);

					printf("Keyspace name: '%.*s'\n", (int)name_length, name);
				}
				//cass_schema_meta_free(schema_meta);
			}
		}

		if(strcmp(cmd, "list") == 0){
			//LIST FUCNTION
			printf("Entering list function.\n");
		}

		if(strcmp(cmd, "use") == 0){
			//USE FUCNTION
			char keyspace[BUFSIZE], table[BUFSIZE];
			nextarg(cmdline, &pos, " ", keyspace);
			nextarg(cmdline, &pos, " ", table);
			printf("Entering use function with params %s and %s.\n",keyspace, table);
		}

		if(strcmp(cmd, "get") == 0){
			//GET FUCNTION
			char key[BUFSIZE];
			nextarg(cmdline, &pos, " ", key);
			printf("Entering get function with param %s.\n",key);
		}

		if(strcmp(cmd, "insert") == 0){
			//INSERT FUCNTION
			char key[BUFSIZE], value[BUFSIZE];
			nextarg(cmdline, &pos, " ", key);
			nextarg(cmdline, &pos, " ", value);
			printf("Entering insert function with params %s and %s.\n",key, value);
		}


	}
}

int
main(int argc, char**argv)
{
	cli();
	exit(0);
}
