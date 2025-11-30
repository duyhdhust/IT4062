/* * File: common.h
 * Description: Header file chứa toàn bộ định nghĩa giao thức và cấu trúc dữ liệu.
 * Đã chuẩn hóa theo đề bài "Ứng dụng mua bán vé xem phim online".
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFF_SIZE 2048 // Tăng buffer lên chút để chứa danh sách phim dài

// ====================================================
//                  RESPONSE CODES (MÃ TRẢ VỀ)
// ====================================================

// --- 1. KẾT NỐI & CÚ PHÁP (Hệ thống) ---
#define RES_CONNECT_SUCCESS   "100" // Kết nối thành công
#define RES_SYNTAX_ERROR      "300" // Cú pháp sai

// --- 2. XÁC THỰC (Account Management) ---
// Yêu cầu: Đăng ký, Đăng nhập, Quản lý phiên, Kiểm soát quyền
#define RES_LOGIN_SUCCESS     "110" // Đăng nhập OK
#define RES_LOGOUT_SUCCESS    "130" // Đăng xuất OK
#define RES_REGISTER_SUCCESS  "150" // Đăng ký thành công
#define RES_REGISTER_EXIST    "151" // Username đã tồn tại

#define RES_ACCOUNT_LOCKED    "211" // Tài khoản bị khóa
#define RES_USER_NOT_FOUND    "212" // Không tồn tại
#define RES_WRONG_PASSWORD    "213" // Sai mật khẩu
#define RES_LOGGED_ELSEWHERE  "214" // Đang đăng nhập nơi khác (Quản lý phiên)
#define RES_ALREADY_LOGGED_IN "215" // Đã đăng nhập rồi
#define RES_NOT_LOGGED_IN     "221" // Chưa đăng nhập

// --- 3. TÌM KIẾM & DUYỆT PHIM (User Features) ---
// Yêu cầu: Tìm tên, Duyệt thể loại, Duyệt rạp, Duyệt giờ
#define RES_SEARCH_FOUND      "310" // Tìm thấy phim
#define RES_SEARCH_NOT_FOUND  "311" // Không tìm thấy

#define RES_LIST_GENRE_OK     "320" // Duyệt theo thể loại OK
#define RES_LIST_CINEMA_OK    "321" // Duyệt theo rạp OK
#define RES_LIST_TIME_OK      "322" // Duyệt theo giờ OK

// --- 4. ĐẶT VÉ (Booking Features) ---
// Yêu cầu: Chọn phim, Chọn suất, Xem map ghế, Đặt nhiều ghế
#define RES_SELECT_MOVIE_OK   "330" // Chọn phim OK
#define RES_MOVIE_NOT_EXIST   "331" // Phim không tồn tại

#define RES_SELECT_SHOW_OK    "340" // Chọn suất chiếu OK
#define RES_SHOW_NOT_EXIST    "341" // Suất chiếu không tồn tại

#define RES_SHOW_SEATS_OK     "350" // Xem bản đồ ghế OK
#define RES_BOOK_SUCCESS      "360" // Đặt ghế thành công
#define RES_BOOK_FAILED       "361" // Ghế đã bị người khác đặt

// --- 5. QUẢN LÝ (Admin Features) ---
// Yêu cầu: Thêm phim, Công bố suất, Sửa suất
#define RES_ADD_MOVIE_OK      "410" // Thêm phim OK
#define RES_ADD_MOVIE_EXIST   "411" // Phim đã tồn tại

#define RES_ADD_SHOW_OK       "420" // Công bố suất chiếu OK
#define RES_ADD_SHOW_FAIL     "421" // Phim không tồn tại để thêm suất

#define RES_EDIT_SHOW_OK      "430" // Sửa suất chiếu OK
#define RES_EDIT_SHOW_FAIL    "431" // Suất chiếu không tồn tại để sửa

#define RES_DELETE_OK         "440" // Xóa OK
#define RES_DELETE_FAIL       "441" // Không tồn tại để xóa

// ====================================================
//                  DATA STRUCTURES
// ====================================================

// 1. Cấu trúc User (Dùng Linked List để quản lý phiên động)
typedef struct UserNode {
    char username[30];
    char password[30];
    int role;      // 0: User, 1: Admin
    
    // Hai trường này chỉ tồn tại trên RAM để quản lý phiên (Session)
    int is_online; // 0: Offline, 1: Online
    int socket_fd; // Socket nào đang kết nối
    
    struct UserNode *next;
} UserNode;

// 2. Cấu trúc Phim
// Yêu cầu: Tiêu đề, mô tả, thời lượng, thể loại (để duyệt)
typedef struct {
    char id[10];           // Mã phim (ví dụ: M01)
    char title[100];       // Tên phim
    char description[200]; // Mô tả
    char genre[30];        // Thể loại (Hành động, Tình cảm...) -> Đã thêm mới
    int duration;          // Thời lượng (phút)
} Movie;

// 3. Cấu trúc Suất chiếu (ShowTime)
// Yêu cầu: Rạp, khung giờ, map ghế
typedef struct {
    char id[10];           // Mã suất chiếu (ví dụ: S01)
    char movie_id[10];     // Mã phim chiếu
    char cinema[50];       // Tên rạp / Phòng chiếu
    char time[20];         // Khung giờ (ví dụ: "19:30")
    
    // Map ghế: 100 ghế (10 hàng x 10 cột)
    // Giá trị: 0 là Trống, 1 là Đã đặt
    int seats[100]; 
} ShowTime;

// 4. Khai báo hàm dùng chung
void load_users_from_file(const char *filename);
void handle_client(int conn_sock);

#endif