/* File: admin.c - Quan ly Admin */
#include "admin.h"
#include "../server/utils.h"

// TODO: Them cac ham DB

void process_add_movie(int sock, PGconn *db_conn, char *payload) {
    // Xu ly API them phim moi
    char response[2048];
    sprintf(response, "999: Add movie - Coming soon\n");
    send(sock, response, strlen(response), 0);
    write_log("Add movie requested");
}
