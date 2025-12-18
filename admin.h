#ifndef ADMIN_H
#define ADMIN_H

#include "common.h"
#include <libpq-fe.h>

// ===== CHỨC NĂNG QUẢN LÝ (ADMIN) =====

// Quản lý phim
void process_add_movie(int sock, PGconn *db_conn, char *payload);
void process_edit_movie(int sock, PGconn *db_conn, char *payload);
void process_delete_movie(int sock, PGconn *db_conn, char *payload);

// Quản lý suất chiếu
void process_add_showtime(int sock, PGconn *db_conn, char *payload);
void process_edit_showtime(int sock, PGconn *db_conn, char *payload);
void process_delete_showtime(int sock, PGconn *db_conn, char *payload);

// Quản lý người dùng (nếu cần)
void process_lock_user(int sock, PGconn *db_conn, char *payload);
void process_unlock_user(int sock, PGconn *db_conn, char *payload);

#endif
