/*
 * File: client.c
 * Description: Cinema Server Client - supports LOGIN, REGISTER, LOGOUT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 4096

/**
 * @function recv_line
 * @brief Receive one complete line (terminated by '\n' or "\r\n") from socket.
 * @param sock The connected socket file descriptor.
 * @param buff Buffer to store received line.
 * @param max_len Maximum number of bytes to read.
 * @return Number of bytes read (excluding null terminator), or -1 on error.
 */
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

/**
 * @function main
 * @brief Cinema Server Client for login, register, logout operations.
 * @param argc Argument count (should be 3).
 * @param argv argv[1] = server IP, argv[2] = server port.
 * @usage
 *     ./client 127.0.0.1 8080
 * @details
 *  - Connects to Cinema Server at specified IP:PORT
 *  - Displays menu for:
 *        1. REGISTER - Create new account
 *        2. LOGIN - Sign in with username/password
 *        3. LOGOUT - Sign out
 *        4. EXIT - Close connection
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8080\n", argv[0]);
        exit(1);
    }

    char *sv_ip = argv[1];
    int sv_port = atoi(argv[2]);
    int client_sock;
    struct sockaddr_in server_addr;
    char sendbuf[BUFF_SIZE], recvbuf[BUFF_SIZE];
    int is_logged_in = 0;
    char current_user[50] = "";

    /**
     * Step 1: Create TCP socket
     */
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket error");
        exit(1);
    }

    /**
     * Step 2: Configure server address
     */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(sv_port);
    server_addr.sin_addr.s_addr = inet_addr(sv_ip);

    /**
     * Step 3: Connect to server
     */
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        exit(1);
    }

    /**
     * Step 4: Receive welcome message from server
     */
    if (recv_line(client_sock, recvbuf, BUFF_SIZE) > 0) {
        printf("[SERVER] %s\n", recvbuf);
    }

    /**
     * Step 5: Interactive menu loop
     */
    while (1) {
        printf("\n============================================\n");
        printf("           CINEMA SERVER CLIENT\n");
        printf("============================================\n");
        printf("1. REGISTER - Create new account\n");
        printf("2. LOGIN - Sign in\n");
        printf("3. LOGOUT - Sign out\n");
        printf("4. EXIT - Close connection\n");
        printf("============================================\n");
        printf("Choose: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // consume newline

        /**
         * --- REGISTER command ---
         * Format: REG <username> <password>
         */
        if (choice == 1) {
            char username[50], password[50];
            printf("\n--- REGISTER ---\n");
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);
            getchar(); // consume newline

            sprintf(sendbuf, "REG %s %s\n", username, password);
            send(client_sock, sendbuf, strlen(sendbuf), 0);

            if (recv_line(client_sock, recvbuf, BUFF_SIZE) > 0) {
                printf("[SERVER] %s\n", recvbuf);
                // Parse response code
                if (strncmp(recvbuf, "110", 3) == 0) {
                    printf("✓ Registration successful!\n");
                } else if (strncmp(recvbuf, "111", 3) == 0) {
                    printf("✗ Username already exists!\n");
                }
            }
        }

        /**
         * --- LOGIN command ---
         * Format: LOGIN <username> <password>
         */
        else if (choice == 2) {
            char username[50], password[50];
            printf("\n--- LOGIN ---\n");
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);
            getchar(); // consume newline

            sprintf(sendbuf, "LOGIN %s %s\n", username, password);
            send(client_sock, sendbuf, strlen(sendbuf), 0);

            if (recv_line(client_sock, recvbuf, BUFF_SIZE) > 0) {
                printf("[SERVER] %s\n", recvbuf);
                // Parse response code
                if (strncmp(recvbuf, "120", 3) == 0) {
                    printf("✓ Login successful! Welcome %s\n", username);
                    is_logged_in = 1;
                    strcpy(current_user, username);
                } else if (strncmp(recvbuf, "122", 3) == 0) {
                    printf("✗ User not found!\n");
                } else if (strncmp(recvbuf, "123", 3) == 0) {
                    printf("✗ Wrong password!\n");
                } else if (strncmp(recvbuf, "121", 3) == 0) {
                    printf("✗ Account is locked!\n");
                } else if (strncmp(recvbuf, "124", 3) == 0) {
                    printf("✗ Already logged in!\n");
                }
            }
        }

        /**
         * --- LOGOUT command ---
         * Format: LOGOUT
         */
        else if (choice == 3) {
            printf("\n--- LOGOUT ---\n");
            sprintf(sendbuf, "LOGOUT\n");
            send(client_sock, sendbuf, strlen(sendbuf), 0);

            if (recv_line(client_sock, recvbuf, BUFF_SIZE) > 0) {
                printf("[SERVER] %s\n", recvbuf);
                // Parse response code
                if (strncmp(recvbuf, "130", 3) == 0) {
                    printf("✓ Logout successful!\n");
                    is_logged_in = 0;
                    strcpy(current_user, "");
                } else if (strncmp(recvbuf, "131", 3) == 0) {
                    printf("✗ Not logged in!\n");
                }
            }
        }

        /**
         * --- EXIT ---
         * Close connection and exit
         */
        else if (choice == 4) {
            printf("\n[EXIT] Closing connection...\n");
            close(client_sock);
            exit(0);
        }

        else {
            printf("✗ Invalid choice. Please try again.\n");
        }
    }

    close(client_sock);
    return 0;
}
