/* * File: server.c
 * Description: Main server entry point.
 */

#include "common.h"

// Biến toàn cục (để handler.c dùng chung)
UserNode *head_user = NULL;
pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

// Hàm luồng xử lý cho từng Client
void *client_thread(void *arg) {
    int conn_sock = *((int *)arg);
    free(arg); 
    
    // Gửi thông báo kết nối thành công
    char welcome[100];
    sprintf(welcome, "%s: Connected to Cinema Server\n", RES_CONNECT_SUCCESS);
    send(conn_sock, welcome, strlen(welcome), 0);

    // Chuyển sang xử lý logic (bên handler.c)
    handle_client(conn_sock);

    // Khi client ngắt kết nối đột ngột: Dọn dẹp session
    pthread_mutex_lock(&users_mutex);
    UserNode *curr = head_user;
    while (curr != NULL) {
        if (curr->socket_fd == conn_sock) {
            curr->is_online = 0;
            curr->socket_fd = -1;
            printf("[Log] User '%s' disconnected.\n", curr->username);
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&users_mutex);

    close(conn_sock);
    return NULL;
}

int main() {
    int listen_sock, *conn_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    pthread_t tid;

    // 1. Load dữ liệu tài khoản
    load_users_from_file("account.txt");

    // 2. Tạo Socket
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 3. Cấu hình Socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Tránh lỗi "Address already in use"
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_sock, 10) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Started on port %d. Waiting for connections...\n", PORT);

    // 4. Vòng lặp chấp nhận kết nối
    while (1) {
        conn_sock = malloc(sizeof(int)); 
        if (conn_sock == NULL) continue;

        *conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &sin_size);
        
        if (*conn_sock == -1) {
            perror("Accept failed");
            free(conn_sock);
            continue;
        }

        printf("[Info] New connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Tạo Thread mới (Blocking I/O + Multi-thread)
        if (pthread_create(&tid, NULL, &client_thread, conn_sock) != 0) {
            perror("Thread creation failed");
            free(conn_sock);
        } else {
            pthread_detach(tid); // Tự giải phóng khi xong việc
        }
    }

    close(listen_sock);
    return 0;
}