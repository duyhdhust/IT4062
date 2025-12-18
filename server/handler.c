/* * File: handler.c
 * Description: Command Router - Điều hướng các request đến handler tương ứng
 */

#include "common.h"
#include "../auth/auth.h"
#include "../user/booking.h"
#include "../admin/admin.h"
#include "utils.h"

// Khai báo các hàm DB
PGconn *connect_db();
void disconnect_db(PGconn *conn);

// --- B�?ĐIỀU HƯỚNG ---
void handle_client(int conn_sock) {
    char buff[BUFF_SIZE];
    char response[BUFF_SIZE];
    
    // --- Khởi tạo DB Connection (1 lần duy nhất) ---
    PGconn *db_conn = connect_db();
    if (!db_conn) {
        sprintf(response, "%s: Database connection failed\n", RES_SYNTAX_ERROR);
        send(conn_sock, response, strlen(response), 0);
        return;
    }
    
    // --- Session Cục B�?(Nằm trên Stack của Thread) ---
    // Khi thread kết thúc, các biến này t�?mất -> Không cần dọn dẹp
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
                if (process_login(conn_sock, db_conn, buff + 6, current_user, &current_role)) {
                    is_logged_in = 1;
                }
            }
        } 
        // 2. LOGOUT
        else if (strncmp(buff, "LOGOUT", 6) == 0) {
            process_logout(conn_sock, current_user, &is_logged_in, &current_role);
        }
        // 3. REG
        else if (strncmp(buff, "REG", 3) == 0) {
            if (is_logged_in) {
                sprintf(response, "%s: Please logout first\n", RES_ALREADY_LOGGED_IN);
                send(conn_sock, response, strlen(response), 0);
            } else {
                process_register(conn_sock, db_conn, buff + 4);
            }
        }
        // TODO: Các lệnh khác s�?code tiếp �?đây
        else {
            sprintf(response, "%s: Unknown Command\n", RES_SYNTAX_ERROR);
            send(conn_sock, response, strlen(response), 0);
        }
    }
    
    // --- Cleanup: Đóng DB Connection ---
    disconnect_db(db_conn);
}
