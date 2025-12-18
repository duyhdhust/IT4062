#ifndef AUTH_H
#define AUTH_H

#include "common.h"
#include <libpq-fe.h>

// Authentication functions
void process_register(int sock, PGconn *db_conn, char *payload);
int process_login(int sock, PGconn *db_conn, char *payload, char *out_username, int *out_role);
void process_logout(int sock, char *current_user, int *is_logged_in, int *current_role);

#endif
