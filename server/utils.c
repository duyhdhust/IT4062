/* * File: utils.c
 * Description: Các hàm tiện ích (logging, network helpers)
 */

#include "utils.h"
#include <time.h>

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
