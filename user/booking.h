#ifndef BOOKING_H
#define BOOKING_H

#include "common.h"
#include <libpq-fe.h>

// ===== CHỨC NĂNG NGƯỜI MUA (USER) =====

// Tìm kiếm phim
void process_search_movie(int sock, PGconn *db_conn, char *payload);

// Xem danh sách phim theo thể loại
void process_list_by_genre(int sock, PGconn *db_conn, char *payload);

// Xem danh sách rạp chiếu phim
void process_list_cinemas(int sock, PGconn *db_conn, char *payload);

// Xem lịch chiếu của phim
void process_list_showtimes(int sock, PGconn *db_conn, char *payload);

// Đặt vé
void process_book_ticket(int sock, PGconn *db_conn, char *username, char *payload);

// Xem vé đã đặt
void process_view_my_tickets(int sock, PGconn *db_conn, char *username);

#endif
