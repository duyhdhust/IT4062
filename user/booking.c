/* * File: booking.c
 * Description: X�?lý các API liên quan đến chức năng người mua (User)
 */

#include "booking.h"
#include "../server/utils.h"

// Khai báo các hàm DB (s�?implement sau)
// TODO: Thêm các hàm DB tương ứng trong db.c

// --- API: TÌM KIẾM PHIM ---
// Command: SEARCH <movie_name>
void process_search_movie(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement search logic
    // 1. Parse movie_name t�?payload
    // 2. Gọi db_search_movie(db_conn, movie_name)
    // 3. Format kết qu�?và gửi v�?client
    
    sprintf(response, "%s: Search feature - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
    write_log("Search movie requested");
}

// --- API: XEM PHIM THEO TH�?LOẠI ---
// Command: LIST_GENRE <genre>
void process_list_by_genre(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement list by genre
    // 1. Parse genre t�?payload
    // 2. Gọi db_list_movies_by_genre(db_conn, genre)
    // 3. Format danh sách phim và gửi v�?client
    
    sprintf(response, "%s: List by genre - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: XEM DANH SÁCH RẠP ---
// Command: LIST_CINEMA <movie_id>
void process_list_cinemas(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement list cinemas
    // 1. Parse movie_id t�?payload
    // 2. Gọi db_list_cinemas_by_movie(db_conn, movie_id)
    // 3. Format danh sách rạp và gửi v�?client
    
    sprintf(response, "%s: List cinemas - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: XEM LỊCH CHIẾU ---
// Command: LIST_TIME <movie_id> <cinema_id>
void process_list_showtimes(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement list showtimes
    // 1. Parse movie_id, cinema_id t�?payload
    // 2. Gọi db_list_showtimes(db_conn, movie_id, cinema_id)
    // 3. Format lịch chiếu và gửi v�?client
    
    sprintf(response, "%s: List showtimes - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: ĐẶT VÉ ---
// Command: BOOK <showtime_id> <seat_number>
void process_book_ticket(int sock, PGconn *db_conn, char *username, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement booking
    // 1. Parse showtime_id, seat_number t�?payload
    // 2. Kiểm tra gh�?còn trống: db_check_seat_available()
    // 3. Tạo booking: db_create_booking(db_conn, username, showtime_id, seat)
    // 4. Gửi kết qu�?v�?client
    
    sprintf(response, "%s: Booking feature - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
    write_log("Booking requested");
}

// --- API: XEM VÉ ĐÃ ĐẶT ---
// Command: MY_TICKETS
void process_view_my_tickets(int sock, PGconn *db_conn, char *username) {
    char response[BUFF_SIZE];
    
    // TODO: Implement view tickets
    // 1. Gọi db_get_user_tickets(db_conn, username)
    // 2. Format danh sách vé và gửi v�?client
    
    sprintf(response, "%s: View tickets - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}
