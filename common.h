/* * File: common.h
 * Description: Header file chứa toàn bộ định nghĩa giao thức và cấu trúc dữ liệu.
 * Updated: Khớp 100% với bảng giao thức MỚI (SỬA).
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
#define BUFF_SIZE 2048 

// ====================================================
//                  RESPONSE CODES (MỚI)
// ====================================================

// --- 0. HỆ THỐNG & LỖI CHUNG ---
#define RES_CONNECT_SUCCESS   "100" // Kết nối thành công
#define RES_SYNTAX_ERROR      "999" // Cú pháp sai (Thay cho 300 cũ)

// --- 1. XÁC THỰC (AUTH - 1xx) ---
// Đăng ký
#define RES_REGISTER_SUCCESS  "110" // Đăng ký thành công (Cũ: 150)
#define RES_REGISTER_EXIST    "111" // Username đã tồn tại (Cũ: 151)

// Đăng nhập
#define RES_LOGIN_SUCCESS     "120" // Đăng nhập OK (Cũ: 110)
#define RES_ACCOUNT_LOCKED    "121" // Tài khoản bị khóa (Cũ: 211)
#define RES_USER_NOT_FOUND    "122" // Không tồn tại (Cũ: 212)
#define RES_WRONG_PASSWORD    "123" // Sai mật khẩu (Cũ: 213)
#define RES_LOGGED_ELSEWHERE  "124" // Đang đăng nhập nơi khác (Cũ: 214)

// Đăng xuất / Trạng thái
#define RES_LOGOUT_SUCCESS    "130" // Đăng xuất OK
#define RES_NOT_LOGGED_IN     "131" // Chưa đăng nhập (Cũ: 221)

// --- 2. TÌM KIẾM & DUYỆT (SEARCH/LIST - 2xx) ---
#define RES_SEARCH_FOUND      "200" // Tìm thấy (Cũ: 310)
#define RES_SEARCH_NOT_FOUND  "201" // Không tìm thấy (Cũ: 311)

#define RES_LIST_GENRE_OK     "210" // Duyệt theo thể loại OK
#define RES_LIST_CINEMA_OK    "220" // Duyệt theo rạp OK
#define RES_LIST_TIME_OK      "230" // Duyệt theo giờ OK

// --- 3. CHỌN & ĐẶT VÉ (SELECT/BOOK - 3xx) ---
#define RES_SELECT_MOVIE_OK   "310" // Chọn phim OK (Cũ: 330)
#define RES_MOVIE_NOT_EXIST   "311" // Phim k tồn tại (Cũ: 331)

#define RES_SELECT_SHOW_OK    "320" // Chọn suất chiếu OK (Cũ: 340)
#define RES_SHOW_NOT_EXIST    "321" // Suất chiếu k tồn tại (Cũ: 341)

#define RES_SHOW_SEATS_OK     "330" // Xem bản đồ ghế OK (Cũ: 350)

#define RES_BOOK_SUCCESS      "340" // Đặt ghế thành công (Cũ: 360)
#define RES_BOOK_FAILED       "341" // Ghế đã được đặt (Cũ: 361)
#define RES_BOOK_SEAT_INVALID "342" // Ghế không tồn tại (Mới)
#define RES_BOOK_SHOW_INVALID "343" // Suất chiếu không hợp lệ (Mới)

// --- 4. QUẢN LÝ - THÊM (ADD - 4xx) ---
#define RES_ADD_MOVIE_OK      "410" // Thêm phim OK
#define RES_ADD_MOVIE_EXIST   "411" // Phim đã tồn tại

#define RES_ADD_SHOW_OK       "420" // Công bố suất OK
#define RES_ADD_SHOW_FAIL     "421" // Phim k tồn tại
#define RES_ADD_SHOW_CONFLICT "422" // Trùng lịch chiếu (Mới)
#define RES_ADD_SHOW_CINEMA_ERR "423" // Rạp k tồn tại (Mới)

// --- 5. QUẢN LÝ - SỬA/XÓA (EDIT/DEL - 5xx) ---
// Sửa suất chiếu
#define RES_EDIT_SHOW_OK         "510" // Sửa OK (Cũ: 430)
#define RES_EDIT_SHOW_FAIL       "511" // Suất k tồn tại (Cũ: 431)
#define RES_EDIT_SHOW_HAS_TICKET "512" // Đã có vé k thể sửa (Mới)
#define RES_EDIT_SHOW_CINEMA_ERR "513" // Rạp k tồn tại (Mới)

// Xóa phim
#define RES_DEL_MOVIE_OK         "520" // Xóa phim OK (Cũ: 440)
#define RES_DEL_MOVIE_FAIL       "521" // ID k tồn tại
#define RES_DEL_MOVIE_CONSTRAINT "522" // Có suất chiếu/đã bán vé (Mới)

// Xóa suất chiếu
#define RES_DEL_SHOW_OK          "530" // Xóa suất OK
#define RES_DEL_SHOW_FAIL        "531" // Suất k tồn tại
#define RES_DEL_SHOW_CONSTRAINT  "532" // Đã bán vé (Mới)

// ====================================================
//                  DATA STRUCTURES
// ====================================================

// 1. Cấu trúc User (Linked List)
typedef struct UserNode {
    char username[30];
    char password[30];
    int role;      // 0: User, 1: Admin
    int is_online; // 0: Offline, 1: Online
    int socket_fd; // Socket hiện tại
    struct UserNode *next;
} UserNode;

// 2. Cấu trúc Phim
typedef struct {
    char id[10];
    char title[100];
    char description[200];
    char genre[30]; // Thể loại
    int duration;   // Thời lượng
} Movie;

// 3. Cấu trúc Suất chiếu
typedef struct {
    char id[10];
    char movie_id[10];
    char cinema[50];
    char time[20];
    int seats[100]; // 0: Trống, 1: Đã đặt
} ShowTime;

// Prototype hàm
void load_users_from_file(const char *filename);
void handle_client(int conn_sock);

#endif