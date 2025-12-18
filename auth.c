/* * File: auth.c
 * Description: Xử lý các API liên quan đến Authentication
 */

#include "auth.h"
#include "utils.h"

// Khai báo các hàm DB
int db_check_login(PGconn *conn, char *user, char *pass);
int db_register(PGconn *conn, char *user, char *pass);
void db_update_login_time(PGconn *conn, char *user);

// --- API: ĐĂNG KÝ ---
void process_register(int sock, PGconn *db_conn, char *payload) {
    char username[50], password[50];
    char response[BUFF_SIZE];

    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return;
    }

    int result = db_register(db_conn, username, password);
    if (result == 1) {
        sprintf(response, "%s: Register Success\n", RES_REG_SUCCESS);
        write_log("Register Success");
    } else {
        sprintf(response, "%s: Username exists\n", RES_REG_EXIST);
    }
    send(sock, response, strlen(response), 0);
}

// --- API: ĐĂNG NHẬP ---
int process_login(int sock, PGconn *db_conn, char *payload, char *out_username, int *out_role) {
    char username[50], password[50];
    char response[BUFF_SIZE];
    char log_msg[200];

    if (sscanf(payload, "%s %s", username, password) != 2) {
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return 0;
    }

    // Check trực tiếp với DB
    int status = db_check_login(db_conn, username, password);

    if (status >= 0) {
        // OK -> Lưu thông tin vào biến cục bộ (qua con trỏ)
        strcpy(out_username, username);
        *out_role = status;
        
        db_update_login_time(db_conn, username);

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

// --- API: ĐĂNG XUẤT ---
void process_logout(int sock, char *current_user, int *is_logged_in, int *current_role) {
    char response[BUFF_SIZE];
    char log_msg[200];

    if (!(*is_logged_in)) {
        sprintf(response, "%s: Not logged in\n", RES_NOT_LOGGED_IN);
        send(sock, response, strlen(response), 0);
        return;
    }

    // Ghi log trước khi xóa username
    sprintf(log_msg, "Logout: User '%s'", current_user);
    write_log(log_msg);

    // Xóa session
    *is_logged_in = 0;
    *current_role = -1;
    strcpy(current_user, "");

    sprintf(response, "%s: Logout Success\n", RES_LOGOUT_SUCCESS);
    send(sock, response, strlen(response), 0);
}
