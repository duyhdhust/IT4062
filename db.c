/* * File: db.c
 * Description: Kết nối PostgreSQL
 */

#include "common.h"

// TODO: Điền Connection String của Supabase vào đây
const char *conninfo = "postgresql://postgres.mhccdysgnmzzvoysxikz:IT4062@aws-1-ap-south-1.pooler.supabase.com:5432/postgres";

PGconn *connect_db() {
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    return conn;
}

int db_check_login(PGconn *conn, char *user, char *pass) {
    if (!conn) return -2;

    const char *paramValues[1] = { user };
    PGresult *res = PQexecParams(conn, 
        "SELECT password, role, is_locked FROM users WHERE username = $1",
        1, NULL, paramValues, NULL, NULL, 0);

    int status = -2; 

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        char *db_pass = PQgetvalue(res, 0, 0);
        int role = atoi(PQgetvalue(res, 0, 1));
        char *is_locked = PQgetvalue(res, 0, 2);

        if (strcmp(is_locked, "t") == 0) status = -3;
        else if (strcmp(db_pass, pass) == 0) status = role;
        else status = -1;
    }

    PQclear(res);
    return status;
}

int db_register(PGconn *conn, char *user, char *pass) {
    if (!conn) return 0;

    const char *paramValues[2] = { user, pass };
    PGresult *res = PQexecParams(conn,
        "INSERT INTO users (username, password, role) VALUES ($1, $2, 0)",
        2, NULL, paramValues, NULL, NULL, 0);

    int success = (PQresultStatus(res) == PGRES_COMMAND_OK) ? 1 : 0;
    PQclear(res);
    return success;
}

void db_update_login_time(PGconn *conn, char *username) {
    if (!conn) return;
    const char *paramValues[1] = { username };
    PGresult *res = PQexecParams(conn,
        "UPDATE users SET last_login = NOW() WHERE username = $1",
        1, NULL, paramValues, NULL, NULL, 0);
    PQclear(res);
}

void disconnect_db(PGconn *conn) {
    if (conn) {
        PQfinish(conn);
    }
}