/* ============================================================================
 * File: utils.c
 * Description: Cac ham tien ich (Logging, Network helpers)
 * ============================================================================
 */

#include "utils.h"
#include <time.h>

/* ----------------------------------------------------------------------------
 * Function: write_log
 * Description: Ghi log vao file log.txt voi timestamp
 * Parameters:
 *   - msg: Thong diep can ghi log
 * Returns: void
 * ----------------------------------------------------------------------------
 */
void write_log(char *msg) {
    // Mo file log.txt o che do append (them vao cuoi file)
    FILE *f = fopen("log.txt", "a");
    
    // Neu mo file that bai thi thoat
    if (f == NULL) return;
    
    // Lay thoi gian hien tai (so giay tu 1970)
    time_t now = time(NULL);
    
    // Chuyen thoi gian sang dang chuoi (vi du: "Wed Dec 18 22:30:00 2024\n")
    char *t = ctime(&now);
    
    // Xoa ky tu newline o cuoi chuoi thoi gian
    if (t[strlen(t)-1] == '\n') 
        t[strlen(t)-1] = '\0';
    
    // Ghi log voi format: [timestamp] message
    fprintf(f, "[%s] %s\n", t, msg);
    
    // Dong file
    fclose(f);
}

/* ----------------------------------------------------------------------------
 * Function: recv_line
 * Description: Nhan 1 dong du lieu tu socket (cho den khi gap newline)
 * Parameters:
 *   - sock: Socket descriptor
 *   - buff: Buffer de luu du lieu nhan duoc
 *   - max_len: Kich thuoc toi da cua buffer
 * Returns: So byte da nhan, hoac -1 neu loi
 * ----------------------------------------------------------------------------
 */
int recv_line(int sock, char *buff, int max_len) {
    // Bien dem so byte da nhan
    int n, i = 0;
    
    // Bien luu tung byte nhan duoc
    char c;
    
    // Doc tung byte cho den khi gap newline hoac day buffer
    while (i < max_len - 1) {
        // Nhan 1 byte tu socket
        // recv(socket, buffer, size, flags)
        n = recv(sock, &c, 1, 0); 
        
        // Neu loi hoac ket noi bi ngat
        if (n <= 0) return -1; 
        
        // Luu byte vao buffer
        buff[i++] = c;
        
        // Neu gap newline thi dung
        if (c == '\n') break;
    }
    
    // Ket thuc chuoi bang null terminator
    buff[i] = '\0';
    
    // Xoa cac ky tu \r va \n o cuoi chuoi
    // strcspn(str, chars) tra ve vi tri dau tien cua chars trong str
    buff[strcspn(buff, "\r\n")] = 0; 
    
    // Tra ve so byte da nhan
    return i;
}
