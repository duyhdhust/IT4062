/* * File: server.c
 * Description: Server Main Loop 
 */

#include "common.h"

// Forward declaration
void handle_client(int conn_sock);

// Thread xử lý từng client
void *client_thread(void *arg) {
    int conn_sock = *((int *)arg);
    free(arg); 
    
    // Gửi chào mừng
    char welcome[100];
    sprintf(welcome, "%s: Connected to Cinema Server\n", RES_CONNECT_SUCCESS);
    send(conn_sock, welcome, strlen(welcome), 0);

    // Chuyển quyền xử lý sang handler.c
    // Khi hàm này return nghĩa là client đã ngắt kết nối
    handle_client(conn_sock);

    close(conn_sock);
    printf("[Info] Client disconnected.\n");
    return NULL;
}

int main() {
    int listen_sock, *conn_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    pthread_t tid;

    // 1. Tạo Socket
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Cấu hình Socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

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

    printf("[Server] Started on port %d.\n", PORT);

    // 3. Vòng lặp chính
    while (1) {
        conn_sock = malloc(sizeof(int)); 
        if (!conn_sock) continue;

        *conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &sin_size);
        
        if (*conn_sock == -1) {
            perror("Accept failed");
            free(conn_sock);
            continue;
        }

        printf("[Info] New connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (pthread_create(&tid, NULL, &client_thread, conn_sock) != 0) {
            perror("Thread creation failed");
            free(conn_sock);
        } else {
            pthread_detach(tid);
        }
    }

    close(listen_sock);
    return 0;
}