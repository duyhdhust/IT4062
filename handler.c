/* * File: handler.c
 * Description: Logic nghiệp vụ, quản lý phiên CỤC BỘ.
 */

#include "common.h"
#include <time.h>

// Khai báo các hàm DB
int db_check_login(char *user, char *pass);
int db_register(char *user, char *pass);
void db_update_login_time(char *user);

// --- HÀM GHI LOG ---
void write_log(char *msg) {
    FILE *f = fopen("log.txt", "a");
    if (f == NULL) return;
    time_t now = time(NULL);
    char *t = ctime(&now);
    if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
    fprintf(f, "[%s] %s\n", t, msg);
    fclose(f);
}

// --- XỬ LÝ TRUYỀN DÒNG ---
int recv_line(int sock, char *buff, int max_len) {
    int n, i = 0;
    char c;
    while (i < max_len - 1) {
        n = recv(sock, &c, 1, 0); 
        if (n <= 0) return -1; 
        buff[i++] = c;
        if (c == '\n') break;
    }
    buff[i] = '\0';
    buff[strcspn(buff, "\r\n")] = 0; 
    return i;
}

// --- LOGIC ĐĂNG KÝ ---
void process_register(int sock, char *payload) {
    char username[50], password[50];
    char response[BUFF_SIZE];

    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return;
    }

    int result = db_register(username, password);
    if (result == 1) {
        sprintf(response, "%s: Register Success\n", RES_REG_SUCCESS);
        write_log("Register Success");
    } else {
        sprintf(response, "%s: Username exists\n", RES_REG_EXIST);
    }
    send(sock, response, strlen(response), 0);
}

// --- LOGIC ĐĂNG NHẬP ---
int process_login(int sock, char *payload, char *out_username, int *out_role) {
    char username[50], password[50];
    char response[BUFF_SIZE];
    char log_msg[200];

    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return 0;
    }

    // Check trực tiếp với DB
    int status = db_check_login(username, password);

    if (status >= 0) {
        // OK -> Lưu thông tin vào biến cục bộ (qua con trỏ)
        strcpy(out_username, username);
        *out_role = status;
        
        db_update_login_time(username);

        sprintf(response, "%s: Login OK\n", RES_LOGIN_SUCCESS);
        sprintf(log_msg, "Login Success: User '%s'", username);
        write_log(log_msg);
        
        send(sock, response, strlen(response), 0);
        return 1;
    } 
    else if (status == -1) {
        sprintf(response, "%s: Wrong password\n", RES_WRONG_PASS);
    } 
    else if (status == -2) {
        sprintf(response, "%s: User not found\n", RES_USER_NOT_FOUND);
    } 
    else if (status == -3) {
        sprintf(response, "%s: Account locked\n", RES_ACC_LOCKED);
    }

    send(sock, response, strlen(response), 0);
    return 0;
}

// --- BỘ ĐIỀU HƯỚNG ---
void handle_client(int conn_sock) {
    char buff[BUFF_SIZE];
    char response[BUFF_SIZE];
    
    // --- Session Cục Bộ (Nằm trên Stack của Thread) ---
    // Khi thread kết thúc, các biến này tự mất -> Không cần dọn dẹp
    char current_user[50] = "";
    int current_role = -1; 
    int is_logged_in = 0;

    while (1) {
        int n = recv_line(conn_sock, buff, BUFF_SIZE);
        if (n <= 0) break; 

        // 1. LOGIN
        if (strncmp(buff, "LOGIN", 5) == 0) {
            if (is_logged_in) {
                sprintf(response, "%s: You are already logged in as %s\n", RES_ALREADY_LOGGED_IN, current_user);
                send(conn_sock, response, strlen(response), 0);
            } else {
                if (process_login(conn_sock, buff + 6, current_user, &current_role)) {
                    is_logged_in = 1;
                }
            }
        } 
        // 2. LOGOUT
        else if (strncmp(buff, "LOGOUT", 6) == 0) {
             if (!is_logged_in) {
                 sprintf(response, "%s: Not logged in\n", RES_NOT_LOGGED_IN);
                 send(conn_sock, response, strlen(response), 0);
             } else {
                 is_logged_in = 0;
                 current_role = -1;
                 char log_msg[200];
                 sprintf(log_msg, "Logout: User '%s'", current_user);
                 write_log(log_msg);
                 
                 strcpy(current_user, ""); // Xóa session

                 sprintf(response, "%s: Logout Success\n", RES_LOGOUT_SUCCESS);
                 send(conn_sock, response, strlen(response), 0);
             }
        }
        // 3. REG
        else if (strncmp(buff, "REG", 3) == 0) {
            if (is_logged_in) {
                sprintf(response, "%s: Please logout first\n", RES_ALREADY_LOGGED_IN);
                send(conn_sock, response, strlen(response), 0);
            } else {
                process_register(conn_sock, buff + 4);
            }
        }
        // TODO: Các lệnh khác sẽ code tiếp ở đây
        else {
            sprintf(response, "%s: Unknown Command\n", RES_SYNTAX_ERROR);
            send(conn_sock, response, strlen(response), 0);
        }
    }
}