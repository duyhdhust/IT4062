/* * File: admin.c
 * Description: Xử lý các API liên quan đến chức năng quản lý (Admin)
 */

#include "admin.h"
#include "utils.h"

// Khai báo các hàm DB (sẽ implement sau)
// TODO: Thêm các hàm DB tương ứng trong db.c

// ===== QUẢN LÝ PHIM =====

// --- API: THÊM PHIM ---
// Command: ADD_MOVIE <title> <genre> <duration> <description>
void process_add_movie(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement add movie
    // 1. Parse thông tin phim từ payload
    // 2. Kiểm tra phim đã tồn tại: db_check_movie_exists()
    // 3. Thêm phim: db_add_movie(db_conn, title, genre, duration, desc)
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Add movie - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
    write_log("Add movie requested");
}

// --- API: SỬA PHIM ---
// Command: EDIT_MOVIE <movie_id> <title> <genre> <duration> <description>
void process_edit_movie(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement edit movie
    // 1. Parse movie_id và thông tin mới
    // 2. Kiểm tra phim tồn tại: db_check_movie_exists()
    // 3. Cập nhật: db_update_movie(db_conn, movie_id, ...)
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Edit movie - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: XÓA PHIM ---
// Command: DEL_MOVIE <movie_id>
void process_delete_movie(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement delete movie
    // 1. Parse movie_id
    // 2. Kiểm tra có suất chiếu/vé đã đặt: db_check_movie_has_bookings()
    // 3. Xóa phim: db_delete_movie(db_conn, movie_id)
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Delete movie - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
    write_log("Delete movie requested");
}

// ===== QUẢN LÝ SUẤT CHIẾU =====

// --- API: THÊM SUẤT CHIẾU ---
// Command: ADD_SHOW <movie_id> <cinema_id> <showtime> <available_seats>
void process_add_showtime(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement add showtime
    // 1. Parse thông tin suất chiếu
    // 2. Kiểm tra movie_id, cinema_id hợp lệ
    // 3. Thêm suất chiếu: db_add_showtime()
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Add showtime - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: SỬA SUẤT CHIẾU ---
// Command: EDIT_SHOW <show_id> <showtime> <available_seats>
void process_edit_showtime(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement edit showtime
    // 1. Parse show_id và thông tin mới
    // 2. Kiểm tra có vé đã đặt: db_check_show_has_bookings()
    // 3. Cập nhật: db_update_showtime()
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Edit showtime - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// --- API: XÓA SUẤT CHIẾU ---
// Command: DEL_SHOW <show_id>
void process_delete_showtime(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement delete showtime
    // 1. Parse show_id
    // 2. Kiểm tra có vé đã đặt: db_check_show_has_bookings()
    // 3. Xóa suất chiếu: db_delete_showtime()
    // 4. Gửi kết quả về client
    
    sprintf(response, "%s: Delete showtime - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}

// ===== QUẢN LÝ NGƯỜI DÙNG =====

// --- API: KHÓA TÀI KHOẢN ---
// Command: LOCK_USER <username>
void process_lock_user(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement lock user
    // 1. Parse username
    // 2. Cập nhật is_locked = true: db_lock_user()
    // 3. Gửi kết quả về client
    
    sprintf(response, "%s: Lock user - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
    write_log("Lock user requested");
}

// --- API: MỞ KHÓA TÀI KHOẢN ---
// Command: UNLOCK_USER <username>
void process_unlock_user(int sock, PGconn *db_conn, char *payload) {
    char response[BUFF_SIZE];
    
    // TODO: Implement unlock user
    // 1. Parse username
    // 2. Cập nhật is_locked = false: db_unlock_user()
    // 3. Gửi kết quả về client
    
    sprintf(response, "%s: Unlock user - Coming soon\n", RES_SYNTAX_ERROR);
    send(sock, response, strlen(response), 0);
}
