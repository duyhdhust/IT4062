/* * File: handler.c
 * Description: Xử lý logic nghiệp vụ, Authentication & Logging
 * Updated: Theo bảng giao thức MỚI (Login 12x, Reg 11x, Syntax 999)
 */

#include "common.h"
#include <time.h>

extern UserNode *head_user;
extern pthread_mutex_t users_mutex;

// --- HÀM GHI LOG HỆ THỐNG ---
void write_log(char *msg) {
    FILE *f = fopen("log.txt", "a");
    if (f == NULL) return;
    
    time_t now = time(NULL);
    char *t = ctime(&now);
    // Xóa ký tự xuống dòng thừa của ctime
    if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
    
    fprintf(f, "[%s] %s\n", t, msg);
    fclose(f);
}

// --- 1. XỬ LÝ TRUYỀN DÒNG (STREAM PROCESSING) ---
// Đọc từng byte, ghép lại thành dòng lệnh, cắt bỏ \r\n
int recv_line(int sock, char *buff, int max_len) {
    int n, i = 0;
    char c;
    while (i < max_len - 1) {
        n = recv(sock, &c, 1, 0); 
        if (n <= 0) return -1; // Lỗi hoặc Client ngắt kết nối
        
        buff[i++] = c;
        if (c == '\n') break;  // Gặp ký tự xuống dòng thì dừng
    }
    buff[i] = '\0';
    // Xóa ký tự xuống dòng \r hoặc \n ở cuối chuỗi
    buff[strcspn(buff, "\r\n")] = 0; 
    return i;
}

// --- 2. QUẢN LÝ DATABASE FILE (account.txt) ---
void load_users_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char u[30], p[30];
    int r;
    while (fscanf(f, "%s %s %d", u, p, &r) != EOF) {
        UserNode *newNode = (UserNode*)malloc(sizeof(UserNode));
        strcpy(newNode->username, u);
        strcpy(newNode->password, p);
        newNode->role = r;
        newNode->is_online = 0;
        newNode->socket_fd = -1;
        
        // Chèn vào đầu danh sách (Linked List)
        newNode->next = head_user;
        head_user = newNode;
    }
    fclose(f);
    printf("[System] Users loaded from %s.\n", filename);
    write_log("System: Users loaded from database");
}

void save_new_user(const char *filename, char *u, char *p, int r) {
    FILE *f = fopen(filename, "a");
    if (f) {
        fprintf(f, "\n%s %s %d", u, p, r);
        fclose(f);
    }
}

// --- 3. LOGIC ĐĂNG KÝ (REG) ---
// Mã trả về: 110 (OK), 111 (Exist), 999 (Syntax)
void process_register(int sock, char *payload) {
    char username[30], password[30];
    char response[BUFF_SIZE];
    char log_msg[200];

    // Check cú pháp (Syntax Error -> 999)
    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return;
    }

    int exists = 0;
    pthread_mutex_lock(&users_mutex);
    UserNode *curr = head_user;
    while (curr != NULL) {
        if (strcmp(curr->username, username) == 0) {
            exists = 1;
            break;
        }
        curr = curr->next;
    }

    if (exists) {
        pthread_mutex_unlock(&users_mutex);
        // Trùng username -> 111
        sprintf(response, "%s: Username exists\n", RES_REGISTER_EXIST);
        sprintf(log_msg, "Register Failed: Username '%s' exists", username);
    } else {
        // Thêm vào RAM
        UserNode *newNode = (UserNode*)malloc(sizeof(UserNode));
        strcpy(newNode->username, username);
        strcpy(newNode->password, password);
        newNode->role = 0; // Mặc định là User
        newNode->is_online = 0;
        newNode->next = head_user;
        head_user = newNode;

        // Thêm vào File
        save_new_user("account.txt", username, password, 0);
        
        pthread_mutex_unlock(&users_mutex);
        
        // Thành công -> 110
        sprintf(response, "%s: Register Success\n", RES_REGISTER_SUCCESS);
        sprintf(log_msg, "Register Success: User '%s'", username);
    }
    
    write_log(log_msg);
    send(sock, response, strlen(response), 0);
}

// --- 4. LOGIC ĐĂNG NHẬP (LOGIN) ---
// Mã trả về: 120 (OK), 121-124 (Lỗi), 999 (Syntax)
// Hàm trả về 1 nếu Login OK, 0 nếu thất bại (để handle_client cập nhật state)
int process_login(int sock, char *payload, char *out_username) {
    char username[30], password[30];
    char response[BUFF_SIZE];
    char log_msg[200];

    // Check cú pháp -> 999
    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return 0;
    }

    UserNode *curr = head_user;
    int success = 0;

    pthread_mutex_lock(&users_mutex);
    while (curr != NULL) {
        if (strcmp(curr->username, username) == 0) {
            // Tìm thấy user
            if (strcmp(curr->password, password) == 0) {
                // Đúng pass
                if (curr->is_online) {
                    // Đã đăng nhập nơi khác -> 124
                    sprintf(response, "%s: Logged in elsewhere\n", RES_LOGGED_ELSEWHERE);
                    sprintf(log_msg, "Login Failed: User '%s' already online", username);
                } else {
                    // OK -> 120
                    curr->is_online = 1;
                    curr->socket_fd = sock;
                    sprintf(response, "%s: Login Success\n", RES_LOGIN_SUCCESS);
                    printf("[System] User '%s' logged in.\n", username);
                    
                    strcpy(out_username, username); // Lưu tên user ra ngoài
                    sprintf(log_msg, "Login Success: User '%s'", username);
                    success = 1;
                }
            } else {
                // Sai pass -> 123
                sprintf(response, "%s: Wrong password\n", RES_WRONG_PASSWORD);
                sprintf(log_msg, "Login Failed: User '%s' wrong password", username);
            }
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&users_mutex);

    if (curr == NULL) {
        // Không tìm thấy user -> 122
        sprintf(response, "%s: User not found\n", RES_USER_NOT_FOUND);
        sprintf(log_msg, "Login Failed: Username '%s' not found", username);
    }
    
    write_log(log_msg);
    send(sock, response, strlen(response), 0);
    return success;
}

// --- 5. BỘ ĐIỀU HƯỚNG (ROUTER) ---
void handle_client(int conn_sock) {
    char buff[BUFF_SIZE];
    char response[BUFF_SIZE];
    char log_msg[200];
    
    // Trạng thái phiên làm việc (Session State)
    char current_user[30] = "";
    int is_logged_in = 0;

    while (1) {
        int n = recv_line(conn_sock, buff, BUFF_SIZE);
        if (n <= 0) break; 

        // --- XỬ LÝ LỆNH LOGIN ---
        if (strncmp(buff, "LOGIN", 5) == 0) {
            if (is_logged_in) {
                // Đã login rồi mà gửi LOGIN tiếp -> Coi là sai cú pháp/ngữ cảnh (999)
                sprintf(response, "%s: You are already logged in. Logout first.\n", RES_SYNTAX_ERROR);
                send(conn_sock, response, strlen(response), 0);
            } else {
                if (process_login(conn_sock, buff + 6, current_user)) {
                    is_logged_in = 1;
                }
            }
        } 
        // --- XỬ LÝ LỆNH LOGOUT ---
        else if (strncmp(buff, "LOGOUT", 6) == 0) {
             if (!is_logged_in) {
                 // Chưa login mà gửi LOGOUT -> 131
                 sprintf(response, "%s: Not logged in\n", RES_NOT_LOGGED_IN);
                 send(conn_sock, response, strlen(response), 0);
             } else {
                 // Xử lý logout
                 pthread_mutex_lock(&users_mutex);
                 UserNode *curr = head_user;
                 while (curr != NULL) {
                     if (curr->socket_fd == conn_sock) {
                         curr->is_online = 0;
                         curr->socket_fd = -1;
                         break;
                     }
                     curr = curr->next;
                 }
                 pthread_mutex_unlock(&users_mutex);
                 
                 // Ghi log
                 sprintf(log_msg, "Logout: User '%s'", current_user);
                 write_log(log_msg);
                 
                 // Reset trạng thái
                 is_logged_in = 0;
                 strcpy(current_user, "");
                 
                 // Trả về 130
                 sprintf(response, "%s: Logout Success\n", RES_LOGOUT_SUCCESS);
                 send(conn_sock, response, strlen(response), 0);
             }
        }
        // --- XỬ LÝ LỆNH ĐĂNG KÝ (REG) ---
        else if (strncmp(buff, "REG", 3) == 0) {
            if (is_logged_in) {
                // Đang login mà đòi REG -> Sai ngữ cảnh -> 999
                sprintf(response, "%s: Please logout before registering\n", RES_SYNTAX_ERROR);
                send(conn_sock, response, strlen(response), 0);
            } else {
                process_register(conn_sock, buff + 4);
            }
        }
        // --- CÁC LỆNH KHÁC (Chưa implement tuần này) ---
        else {
            // Lệnh lạ -> 999
            sprintf(response, "%s: Unknown Command\n", RES_SYNTAX_ERROR);
            send(conn_sock, response, strlen(response), 0);
        }
    }
}