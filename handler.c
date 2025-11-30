/* * File: handler.c
 * Description: Xử lý logic nghiệp vụ + Ghi Log
 */

#include "common.h"
#include <time.h> // Thêm thư viện thời gian

extern UserNode *head_user;
extern pthread_mutex_t users_mutex;

// --- HÀM GHI LOG (Mới thêm) ---
void write_log(char *msg) {
    FILE *f = fopen("log.txt", "a"); // Mở file log.txt chế độ append (ghi nối đuôi)
    if (f == NULL) return;

    time_t now = time(NULL);
    char *t = ctime(&now);
    t[strlen(t) - 1] = '\0'; // Xóa ký tự xuống dòng của hàm ctime

    fprintf(f, "[%s] %s\n", t, msg); // Ghi log có ngày giờ
    fclose(f);
}

// --- 1. XỬ LÝ TRUYỀN DÒNG ---
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

// --- 2. QUẢN LÝ FILE ---
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
        newNode->next = head_user;
        head_user = newNode;
    }
    fclose(f);
    printf("[Data] Users loaded from file.\n");
    write_log("System: Users loaded from database");
}

void save_new_user(const char *filename, char *u, char *p, int r) {
    FILE *f = fopen(filename, "a");
    if (f) {
        fprintf(f, "\n%s %s %d", u, p, r);
        fclose(f);
    }
}

// --- 3. LOGIC REGISTER ---
void process_register(int sock, char *payload) {
    char username[30], password[30];
    char response[BUFF_SIZE];
    char log_msg[200];

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
        sprintf(response, "%s: Username exists\n", RES_REGISTER_EXIST);
        sprintf(log_msg, "Register Failed: Username '%s' exists", username);
    } else {
        UserNode *newNode = (UserNode*)malloc(sizeof(UserNode));
        strcpy(newNode->username, username);
        strcpy(newNode->password, password);
        newNode->role = 0;
        newNode->is_online = 0;
        newNode->next = head_user;
        head_user = newNode;

        save_new_user("account.txt", username, password, 0);
        pthread_mutex_unlock(&users_mutex);
        
        sprintf(response, "%s: Register Success\n", RES_REGISTER_SUCCESS);
        sprintf(log_msg, "Register Success: User '%s'", username);
    }
    
    write_log(log_msg); // Ghi log kết quả
    send(sock, response, strlen(response), 0);
}

// --- 4. LOGIC LOGIN ---
int process_login(int sock, char *payload, char *out_username) {
    char username[30], password[30];
    char response[BUFF_SIZE];
    char log_msg[200];

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
            if (strcmp(curr->password, password) == 0) {
                if (curr->is_online) {
                    sprintf(response, "%s: Logged in elsewhere\n", RES_LOGGED_ELSEWHERE);
                    sprintf(log_msg, "Login Failed: User '%s' already online", username);
                } else {
                    curr->is_online = 1;
                    curr->socket_fd = sock;
                    sprintf(response, "%s: Login Success\n", RES_LOGIN_SUCCESS);
                    printf("[Log] User '%s' logged in.\n", username);
                    strcpy(out_username, username);
                    sprintf(log_msg, "Login Success: User '%s'", username);
                    success = 1;
                }
            } else {
                sprintf(response, "%s: Wrong password\n", RES_WRONG_PASSWORD);
                sprintf(log_msg, "Login Failed: User '%s' wrong password", username);
            }
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&users_mutex);

    if (curr == NULL) {
        sprintf(response, "%s: User not found\n", RES_USER_NOT_FOUND);
        sprintf(log_msg, "Login Failed: Username '%s' not found", username);
    }
    
    write_log(log_msg); // Ghi log
    send(sock, response, strlen(response), 0);
    return success;
}

// --- 5. ROUTER ---
void handle_client(int conn_sock) {
    char buff[BUFF_SIZE];
    char response[BUFF_SIZE];
    char log_msg[200];
    
    char current_user[30] = "";
    int is_logged_in = 0;

    while (1) {
        int n = recv_line(conn_sock, buff, BUFF_SIZE);
        if (n <= 0) break; 

        if (strncmp(buff, "LOGIN", 5) == 0) {
            if (is_logged_in) {
                sprintf(response, "%s: Already logged in as %s\n", RES_ALREADY_LOGGED_IN, current_user);
                send(conn_sock, response, strlen(response), 0);
            } else {
                if (process_login(conn_sock, buff + 6, current_user)) {
                    is_logged_in = 1;
                }
            }
        } 
        else if (strncmp(buff, "LOGOUT", 6) == 0) {
             if (!is_logged_in) {
                 sprintf(response, "%s: Not logged in\n", RES_NOT_LOGGED_IN);
                 send(conn_sock, response, strlen(response), 0);
             } else {
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
                 
                 sprintf(log_msg, "Logout: User '%s'", current_user);
                 write_log(log_msg); // Ghi log logout
                 
                 is_logged_in = 0;
                 strcpy(current_user, "");
                 sprintf(response, "%s: Logout Success\n", RES_LOGOUT_SUCCESS);
                 send(conn_sock, response, strlen(response), 0);
             }
        }
        else if (strncmp(buff, "REG", 3) == 0) {
            if (is_logged_in) {
                sprintf(response, "%s: Please logout first\n", RES_ALREADY_LOGGED_IN);
                send(conn_sock, response, strlen(response), 0);
            } else {
                process_register(conn_sock, buff + 4);
            }
        }
        else {
            sprintf(response, "%s: Unknown Command\n", RES_SYNTAX_ERROR);
            send(conn_sock, response, strlen(response), 0);
        }
    }
}